#!/usr/bin/env python
import angr
import claripy
import sys
from IPython import embed

BASEADDR_CODE = 0x04000000

#fname = sys.argv[1]
fname = "gadgets"
load_options={
    "main_opts": {
        "custom_arch": "AMD64",
        "backend": "blob",
        "custom_base_addr": BASEADDR_CODE,
        "custom_entry_point": 0,
    },
}

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
    return state

def depends_only_on_buf(symvar):
    for v in symvar.variables:
        if v.startswith("BUF_") == False:
            return False
    return True

def check_constraints(state):
    for constraint in state.se.constraints:
        if depends_only_on_buf(constraint) == False:
            return False
    return True

symbuf = get_symbuf(4096)
prj = angr.Project(fname, load_options=load_options)
initial_state = get_initial_state(prj, symbuf)
candidates = []
for offset in range(prj.loader.main_bin.get_min_addr(),
                    prj.loader.main_bin.get_max_addr()):
    state = initial_state.copy()
    pg = prj.factory.path_group(state, save_unconstrained=True)
    pg.step(addr=BASEADDR_CODE+offset)
    while len(pg.active) > 0:
        pg.step()
    for upath in pg.unconstrained:
        if check_constraints(upath.state):
            candidates.append(upath)
embed()

