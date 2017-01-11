#!/usr/bin/env python
import sys
import os
import binascii
from gadget_generator import generate_gadgets
from subprocess import Popen, PIPE
import signal

def fill(buf, ch, size):
    return buf + ch * (size - len(buf))

def print_msg(msg):
    sys.stdout.write("%s\n" % msg)
    sys.stdout.flush()

def timeout():
    print_msg("Timeout")
    sys.exit(-1)

flag = open("flag").read()
def main(argv):
    basedir = os.path.dirname(os.path.abspath(argv[0]))
    os.chdir(basedir)
    rootdir = "subdir"
    # read secret in rootdir
    secret_word = open("%s/secret" % rootdir).read()

    stage = 1
    while stage <= 5:
        print_msg("stage %d/5" % stage)

        # generate gadgets and send them
        gadgets = generate_gadgets()
        encoded_gadgets = binascii.b2a_base64(gadgets).strip()
        print_msg(encoded_gadgets)

        # receive ropchain
        signal.alarm(60)
        encoded_ropchain = sys.stdin.readline()
        signal.alarm(0)
        ropchain = binascii.a2b_base64(encoded_ropchain)
        if len(ropchain) > 4096:
            print_msg("Invalid ROP chain\n")
            break

        child = Popen(
            [
                "./launcher.elf",
                rootdir,
            ],
            stdin=PIPE, stdout=PIPE)
        child.stdin.write(fill(gadgets, '\xCC', 4096))
        child.stdin.write(fill(ropchain, '\xFF', 4096))
        answer_word = child.stdout.read()
        child.wait()
        if secret_word != answer_word:
            print_msg("NG")
            break
        print_msg("OK")
        stage += 1

    if stage >= 6:
        print_msg(flag)

if __name__ == '__main__':
    main(sys.argv)
