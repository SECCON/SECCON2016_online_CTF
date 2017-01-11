import warnings
with warnings.catch_warnings() as w:
    warnings.simplefilter("ignore")
    from Crypto.Util.number import *
from Complex import CZ
import base64
import hashlib
import sys
import os

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

def PoW():
    prefix = base64.b64encode(os.urandom(9))
    print "Send me proof-of-work: sha1hex(prefix + input.rstrip()) = '00000'..."
    print "prefix: %s" % prefix
    sys.stdout.flush()
    x = sys.stdin.readline(16).rstrip()
    if hashlib.sha1(prefix + x).hexdigest()[:5] != "00000":
        print "wrong"
        sys.exit(0)


PoW()

(pk, sk) = genkey(1024)
(N, e) = pk
(p, q, d) = sk


print "N=%d" % N
print "e=%d" % e
sys.stdout.flush()

with open("flag") as f:
    L = size(N)/8-1
    flag = f.readline().rstrip() 
    pad = os.urandom(L-len(flag)-1)
    m = bytes_to_long(pad+"\0"+flag)

C = CZ(N)
y = long(sys.stdin.readline(2048))
print (C(m, y)**e)
