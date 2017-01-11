from Crypto.Util.number import *
from Complex import CZ

def genkey(k):
    e = 0x1337
    while True:
        p = getPrime(k/2)
        q = getPrime(k/2)
        N = p*q
        phi = (q*q-1) * (p*p-1) / GCD(q*q-1, p*p-1)
        if GCD(phi, e) == 1:
            break
    d = inverse(e, phi)
    pk = (N, e)
    sk = (p, q, d)
    return (pk, sk)

(pk, sk) = genkey(1024)
(N, e) = pk
(p, q, d) = sk

with open("publickey", "w") as f:
    f.write(str(N) + "\n")
    f.write(str(e) + "\n")
with open("secretkey", "w") as f:
    f.write(str(p) + "\n")
    f.write(str(q) + "\n")
    f.write(str(d) + "\n")
