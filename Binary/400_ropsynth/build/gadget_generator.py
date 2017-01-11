#!/usr/bin/env python
#from IPython import embed
import random
import sys
from string import Template
from keystone import KS_ARCH_X86, KS_MODE_64, Ks

# for debug
GADGET_GAP_MIN = 1
GADGET_GAP_MAX = 8
CHECK_INSNS_MIN = 1
CHECK_INSNS_MAX = 8
JMPDST_MIN = 1
JMPDST_MAX = 8


def assemble(insn_list):
    ks = Ks(KS_ARCH_X86, KS_MODE_64)
    iarray, cnt = ks.asm("\n".join(insn_list))
    code = ""
    for i in iarray:
        code += chr(i)
    return code


def emit_padding(nmin, nmax):
    return "hlt\n" * random.randint(nmin, nmax)

def emit_arith_insns(regname, nmin, nmax):
    opcodes = ["add", "sub", "xor"]
    all_insns = ""
    for _ in range(random.randint(nmin, nmax)):
        insns = "%s %s, %d" % (
            random.choice(opcodes),
            regname,
            random.randint(0, 0x7fffffff))
        all_insns += insns + "\n"

    return all_insns

lcnt = 0
def emit_check_insns(nmin, nmax):
    global lcnt
    regs = ["rcx", "rbx", "rbp", "r11", "r12", "r13", "r14", "r15"]
    t = Template(
        """
        pop ${regname}
        ${arith_insns}
        cmp ${regname}, ${ival}
        jz ${label}
        hlt
        ${paddings}
        ${label}:
        """)
    all_insns = ""
    for _ in range(random.randint(nmin, nmax)):
        regname = random.choice(regs)
        ival = random.randint(0, 0x7fffffff)
        insns = t.substitute(regname=regname,
                             arith_insns=emit_arith_insns(regname, 1, 8),
                             label="L_ROPSYNTH_%d"%lcnt,
                             ival=ival,
                             paddings=emit_padding(JMPDST_MIN, JMPDST_MAX))
        lcnt += 1
        all_insns += insns

    return all_insns

def generate_gadgets():
    pop_rax = ["pop rax",]
    pop_rsi = ["pop rsi",]
    set_rax_to_rdi = ["push rax", "pop rdi",]
    set_rax_to_rdx = ["push rax", "pop rdx",]
    invoke_syscall = ["syscall",]

    gadget_list = [
        pop_rax,
        pop_rsi,
        set_rax_to_rdi,
        set_rax_to_rdx,
        invoke_syscall,
    ]

    random.shuffle(gadget_list)
    all_gadgets = []
    for gadget in gadget_list:
        all_gadgets.append(emit_padding(GADGET_GAP_MIN, GADGET_GAP_MAX))
        all_gadgets += gadget
        all_gadgets.append(emit_check_insns(CHECK_INSNS_MIN, CHECK_INSNS_MAX))
        all_gadgets.append("ret")

    code = assemble(all_gadgets)
    return code

if __name__ == '__main__':
    debug = False
    if len(sys.argv) == 2:
        debug = True
    if debug:
        random.seed(int(sys.argv[1]))
        """
        random.randint = lambda a,b: a
        random.choice = lambda a: a[0]
        random.shuffle = lambda a: a
        """

    gadgets = generate_gadgets()
    sys.stdout.write(gadgets + '\xcc' * (4096-len(gadgets)))

