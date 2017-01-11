BITS 64
global _start
_start:
L_rax:
pop rax
pop rcx
cmp rcx, 0x7eadbeef
jz L1
hlt
L1:
ret

L_rcx:
pop rcx
ret

L_writemem:
push rax
pop qword [rcx]
ret

L_rsi:
pop rsi
ret

L_sycall:
syscall
ret

L_rax2rdx:
push rax
L_rdx:
pop rdx
ret

L_rax2rdi:
push rax
L_rdi:
pop rdi
ret
