plain = open('./blocklist.xml').read()
enc = open('./blocklist.xml.enc').read()
flag_enc = open('./flag.jpg.enc').read()

assert len(plain) == len(enc)

key = "".join([chr(ord(p)^ord(e)) for p, e in zip(plain, enc)])
key = key[:1024]

open('key', 'w').write(key)

flag = ""
for i in range(len(flag_enc)):
    flag += chr(ord(flag_enc[i])^ord(key[i%1024]))

open('flag.jpg', 'w').write(flag)
