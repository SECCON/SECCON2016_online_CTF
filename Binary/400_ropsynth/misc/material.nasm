BITS 64
global _start
_start:
sys_open  equ 2
sys_read  equ 0
sys_write equ 1
sys_exit  equ 60

mov rsp, L_ropchain
ret

L_rax:
pop rax
ret

L_rcx:
pop rcx
ret

L_writemem:
;mov [rcx], rax
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

SECTION .data
buf: times 64 db 0

L_ropchain:
;set 'flag' to buf
dq L_rax
dq 'flag'
dq L_rcx
dq buf
dq L_writemem

; fd = open(buf, 0, 0)
dq L_rdi
dq buf
dq L_rsi
dq 0
dq L_rdx
dq 0
dq L_rax
dq sys_open
dq L_sycall

; len = read(fd, buf, 64)
dq L_rax2rdi
dq L_rsi
dq buf
dq L_rdx
dq 64
dq L_rax
dq sys_read
dq L_sycall

; write(1, buf, len)
dq L_rdi
dq 1
dq L_rsi
dq buf
dq L_rax2rdx
dq L_rax
dq sys_write
dq L_sycall

; exit()
dq L_rax
dq sys_exit
dq L_sycall

