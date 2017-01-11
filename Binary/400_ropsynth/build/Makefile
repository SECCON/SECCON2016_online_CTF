launcher.elf: launcher.c
	gcc -static -o $@ $<

clean::
	rm -f launcher.elf

files::
	tar cvfz ../files/dist.tgz launcher.c launcher.elf ropsynth.py seccomp-bpf.h Makefile start_server.sh
