#include "colors.h"
#include "util.c"

__asm__(".code16gcc");
__asm__("jmp mbrmain");

#define MAX 2016

void mbrmain(void) {
	int i;

	set_video_mode();
	set_bg_color(TEXT_COLOR_BLACK);
	print("Welcome to SECCON2016!!\r\n", TEXT_COLOR_YELLOW);

	for(i = 0; i < MAX; i++) {
		print("\rChecking ", TEXT_COLOR_WHITE);
		printi(i+1, TEXT_COLOR_WHITE);
		putchar('/', TEXT_COLOR_WHITE);
		printi(MAX, TEXT_COLOR_WHITE);
		print(" files", TEXT_COLOR_WHITE);
	}

	print("\r\nChecking done. Press any key to continue...", TEXT_COLOR_WHITE);

	getkey();

	int cur = 0;
	set_video_mode();
	set_bg_color(TEXT_COLOR_BLACK);
	while (1) {
		print("\rCan you find the flag? :)", cur%0x10);
		cur++;
	}

	stop();
}
