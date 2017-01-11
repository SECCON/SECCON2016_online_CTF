#!/usr/bin/env python
import angr, angrop
from IPython import embed
import sys

"""
p = angr.Project("./a.o",
            load_options={
                "main_opts": {
                    "custom_arch"       : "AMD64",
                    "backend"           : "blob",
                    "custom_base_addr"  : 0xdeadbeef,
                    "custom_entry_point": 0xdeadbeef,
                },
            })
"""
p = angr.Project(sys.argv[1])
print "ROP"
rop = p.analyses.ROP()
print "find_gadgets"
rop.find_gadgets_single_threaded()
embed()
#rop.do_syscall(1,[])
