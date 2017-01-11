CC = gcc
LD = ld
AS = as

CFLAGS = -m32 -ffreestanding -fno-common -fno-builtin -fomit-frame-pointer -masm=intel -O2
ASFLAGS = --32 -fno-common -fno-builtin -fomit-frame-pointer
LDFLAGS = -m elf_i386 -s -static -nostdlib -nmagic --oformat binary

.PHONY: all

all: link write

link: boot main
	$(LD) $(LDFLAGS) -Tlinker.ld -o mbr

boot: boot.S
	$(AS) $(ASFLAGS) -c -o boot.o boot.S

main: main.c
	$(CC) $(CFLAGS) -c -o main.o main.c

write:
#	sudo ./writedisk.sh

clean:
	rm -f bin *.o
