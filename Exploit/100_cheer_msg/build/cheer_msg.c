// gcc -m32 cheer_msg.c -o cheer_msg
#include <stdio.h>
#include <string.h>
#include <alloca.h>
#define BUF_SIZE 64

__attribute__((constructor))
init(){
	setbuf(stdout, NULL);
}

void message(char *buf, int len);
int getnline(char *buf, int len);
int getint(void);


int main(void){
	char *msg_buf;
	int len;

	printf(	"Hello, I'm Nao.\n"
		"Give me your cheering messages :)\n"
		"\n"
		"Message Length >> ");
	len = getint();
	msg_buf = alloca(len);
	
	message(msg_buf, len);
	asm volatile("leave\nret");
}

void message(char *buf, int len){
	char name[BUF_SIZE];

	printf("Message >> ");
	getnline(buf, len);

	printf(	"\n"
		"Oops! I forgot to ask your name...\n"
		"Can you tell me your name?\n"
		"\n"
		"Name >> ");
	getnline(name, sizeof(name));

	printf(	"\n"
		"Thank you %s!\n"
		"Message : %s\n", name, buf);
}

int getnline(char *buf, int len){
        char *lf;

        fgets(buf, len, stdin);
        if(lf=strchr(buf,'\n'))
                *lf='\0';

        return strlen(buf);
}

int getint(void){
        char buf[BUF_SIZE];

        getnline(buf, sizeof(buf));
        return atoi(buf);
}

