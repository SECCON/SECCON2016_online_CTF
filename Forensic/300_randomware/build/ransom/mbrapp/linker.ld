/* ENTRY(start); */
SECTIONS
{
	. = 0x7c00;
	.boot : { boot.o; }
	. = 0x7dfe;
	.sig : { SHORT(0xaa55); }
	. = 0x8000;
	.text : { main.o; }
}
