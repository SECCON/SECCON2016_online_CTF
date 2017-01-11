all: libencrypt.so libdecrypt.so cipher

libencrypt.so: encrypt.c
	$(CC) -O3 -o libencrypt.so -shared -fPIC encrypt.c -Wl,--version-script=libencrypt.version
	strip libencrypt.so

libdecrypt.so: decrypt.c
	$(CC) -O3 -o libdecrypt.so -shared -fPIC decrypt.c -Wl,--version-script=libdecrypt.version

cipher: cipher.c libdecrypt.so libencrypt.so
	$(CC) cipher.c -O3 -ldecrypt -lencrypt -L./ -o cipher

clean:
	$(RM) *.so cipher
