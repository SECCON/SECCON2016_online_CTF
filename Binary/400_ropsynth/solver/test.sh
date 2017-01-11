#!/bin/sh
../challenge/gadget_generator.py $1 > gadgets.$1
./solve.py gadgets.$1 ropchain.$1
cat gadgets.$1 ropchain.$1 | ./launcher.elf
