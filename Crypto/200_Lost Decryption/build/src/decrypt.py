import struct
from unicorn import *
import sys
from unicorn.x86_const import *
with open('libencrypt.so', 'rb') as f:
    bin = f.read()
code = bin
box_start = 0x700
box_end = 0x873
try:
    mu = Uc(UC_ARCH_X86, UC_MODE_64)

    mu.mem_map(0x0, 0x2000)

    # write machine code to be emulated to memory
    mu.mem_write(0x0, code)
    def box(x, k):
        mu.reg_write(UC_X86_REG_RSP, 0x2000)
        mu.reg_write(UC_X86_REG_RDI, x)
        mu.reg_write(UC_X86_REG_RSI, k)

        mu.emu_start(box_start, box_end)
        return struct.unpack("<Q", struct.pack("<q", mu.reg_read(UC_X86_REG_RAX)))[0]
    def decrypt(enc, key):
        m1, m2 = struct.unpack("<QQ", enc)
        k1, k2 = struct.unpack("<QQ", key)
        keys = []
        for i in xrange(0, 14):
            keys.append(k2)
            k2 = box(k2, 10449751217102183504)
            k1, k2 = k2, k1
        for i in xrange(0, 14):
            m1 ^= box(m2, keys[14 - i - 1])
            m1, m2 = m2, m1
        return struct.pack("<QQ", m1, m2)
    key = open("../lost_decryption/key.bin", "rb").read()
    flag = open("../lost_decryption/flag.enc", "rb").read()
    for i in xrange(0, len(flag), 16):
        sys.stdout.write(decrypt(flag[i:i + 16], key))
except UcError as e:
    print("ERROR: %s" % e)
    print('%x' % mu.reg_read(UC_X86_REG_RIP))
