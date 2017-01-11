%.elf : %.o
	gcc -nostdlib -znoexecstack -o $@ $<
	strip $@

%.o : %.nasm
	#nasm -felf64 -w-number-overflow -o $@ $<
	nasm -felf64 -o $@ $<

all: a.elf readflag.elf material.elf gadgets.elf simple_gadgets.elf
