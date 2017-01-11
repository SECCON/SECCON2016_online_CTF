  .file "boot.S"


  .code16
  .intel_syntax noprefix
  .globl start


DRIVE_NUM = 0x7df0

start:
  cli
  xor eax, eax
  mov ss, ax
  mov es, ax
  mov ds, ax
  mov sp, 0x7c00
  sti
  mov [DRIVE_NUM], dl
  mov eax, 0x100
  mov ebx, 0x2
  mov ecx, 0x8000

loop1:
  call read_sector
  dec eax
  cmp eax, 0
  jnz loop1

  mov eax, [0x8000]
  jmp 0x0:0x8000

stop:
  hlt
  jmp stop


read_sector:
  push eax
  xor eax, eax
  push dx
  push si
  push di
  push eax
  push ebx
  mov di, sp
  push eax
  push ebx
  push es
  push cx
  push 0x1
  push 0x10
  mov si, sp
  mov dl, [DRIVE_NUM]
  mov ah, 0x42
  int 0x13
  mov sp, di
  pop ebx
  pop eax
  jnc break
  push ax
  xor ah, ah
  int 0x13
  pop ax
  jmp read_sector

break:
  add ebx, 0x1
  adc eax, 0x0
  add cx, 0x200
  jnc skip1
  mov dx, es
  add dh, 0x10
  mov es, dx

skip1:
  pop di
  pop si
  pop dx
  pop eax
  ret
