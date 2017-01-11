#include "types.h"
#include "colors.h"

__asm__(".code16gcc");

void set_video_mode(void) {
	__asm__ __volatile__(
		"mov ah, 0x00\n"
		"mov al, 0x12\n"
		"int 0x10\n"
	);
}

void set_bg_color(uint16_t color) {
	__asm__ __volatile__(
		"mov ah, 0x0b\n"
		"int 0x10\n"
		: : "b"(color)
	);
}

void print(const char *s, uint16_t color) {
	for(;*s;s++) {
		__asm__ __volatile__("int 0x10" : : "a"(0x0E00|*s), "b"(color&0x00ff));
	}
}

void putchar(const char c, uint16_t color) {
	__asm__ __volatile__("int 0x10" : : "a"(0x0E00|c), "b"(color&0xff));
}


void printi(int n, uint16_t color) {
	if(n) {
		printi(n/10, color);
		putchar((n%10)+'0', color);
	}
}

void movecursor(uint16_t x, uint16_t y) {
	__asm__ __volatile__("int 0x10" : : "a"(0x0200), "b"(0x0000), "d"(y<<8|x));
}


int getkey(void) {
	int ret;
	__asm__ __volatile__("int 0x16" : "=a"(ret) : "a"(0x0000));
}

void reboot(void) {
	__asm__ __volatile__(
		"mov ah, 0x00\n"
		"int 0x16\n"
		"int 0x19\n"
	);
}

void stop(void) {
	while(1) {
		__asm__ ("hlt");
	}
}
