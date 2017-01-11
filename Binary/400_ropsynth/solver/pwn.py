import telnetlib
import subprocess
import socket
from socket import AF_INET, AF_UNIX, SOCK_STREAM
import sys
import os
import glob

######################################################################
# embed ipython shell
######################################################################
import IPython
if hasattr(IPython, "get_ipython") and IPython.get_ipython() == None:
    ipshell = IPython.terminal.embed.InteractiveShellEmbed(display_banner=False)
else:
    ipshell = lambda *x,**y: None

######################################################################
# extended socket
######################################################################
class mysocket(socket.socket):
    def __init__(self, family=AF_INET, type=SOCK_STREAM, proto=0, _sock=None):
        socket.socket.__init__(self, family, type, proto, _sock)

    def recv_until(self, eos):
        retval = ""
        while True:
            ch = self.recv(1)
            if len(ch) == 1:
                retval += ch
            else:
                break
            if retval.endswith(eos):
                break
        return retval

    def recv_lines(self, num=0, prefix="[{num:>4}] "):
        cnt = 0
        retval = ""
        while num <= 0 or cnt < num:
            linebuf = self.recv_until("\n")
            if len(linebuf) > 0:
                retval += linebuf
                print prefix.format(num=cnt), linebuf,
            if linebuf.endswith("\n") == False:
                print
                break
            cnt += 1
        return retval

    def interact(self):
        t = telnetlib.Telnet()
        t.sock = self
        t.interact()

def mkproc(args):
    sp = socket.socketpair(AF_UNIX, SOCK_STREAM)
    fd = sp[0].makefile("rw")
    subprocess.Popen(args, stdin=fd, stdout=fd, stderr=fd, close_fds=True)
    return mysocket(_sock=sp[1])

def mktcp(addr):
    sock = mysocket(AF_INET, SOCK_STREAM)
    sock.connect(addr)
    return sock

######################################################################
# shellcode
######################################################################
shellcode = {}
_scriptdir = os.path.dirname(os.path.abspath(__file__))
_shellcodefiles = os.path.normpath(os.path.join(_scriptdir, "shellcode/*.raw"))
for _fullpath in glob.glob(_shellcodefiles):
    _fname = os.path.splitext(os.path.split(_fullpath)[1])[0]
    with open(_fullpath, "rb") as _f:
        shellcode[_fname] = _f.read()

######################################################################
# other utils
######################################################################
def hexdump(buf):
    retval = ""
    lines, last_bytes = divmod(len(buf), 16)
    for cnt in range(lines):
        linebuf = "%06x:" % (cnt*16)
        for ch in buf[16*cnt:16*(cnt+1)]:
            linebuf += " %02x" % ord(ch)
        linebuf += "  "
        linebuf += "".join(
            [ch  if ord(ch)>=0x20 and ord(ch)<0x7f else "."
             for ch in buf[16*cnt:16*(cnt+1)]])
        linebuf += "\n"
        retval += linebuf

    if last_bytes > 0:
        linebuf = "%06x:" % (lines*16)
        for ch in buf[16*lines:]:
            linebuf += " %02x" % ord(ch)
        for cnt in range(16-last_bytes):
            linebuf += "   "
        linebuf += "  "
        linebuf += "".join(
            [ch  if ord(ch)>=0x20 and ord(ch)<0x7f else "."
             for ch in buf[16*lines:]])
        retval += linebuf

    return retval


######################################################################
# test
######################################################################
def test():
    sock = mkproc(["/bin/ls", "-la"])
    print hexdump(sock.recv_lines())
    ipshell()

if __name__ =="__main__":
    test()
