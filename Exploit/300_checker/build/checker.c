// gcc -fstack-protector-all -Wl,-z,now,-z,relro checker.c -o checker
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#define BUF_SIZE 128

char flag[BUF_SIZE];
char name[BUF_SIZE];

void read_flag(char *fname);
int getaline(char *buf);

__attribute__((constructor))
init(){
	read_flag("flag.txt");
}

void read_flag(char *fname){
	int fd;

	if((fd = open(fname, O_RDONLY))==-1){
		perror(fname);
		_exit(-1);
	}

	read(fd, flag, sizeof(flag));
	close(fd);
}

int main(void){
	char buf[BUF_SIZE];

	dprintf(STDOUT_FILENO, "Hello! What is your name?\nNAME : ");
	getaline(name);

	do{
		dprintf(STDOUT_FILENO, "\nDo you know flag?\n>> ");
		getaline(buf);
	}while(strcmp(buf, "yes"));

	dprintf(STDOUT_FILENO, "\nOh, Really??\nPlease tell me the flag!\nFLAG : ");
	getaline(buf);

	if(!strlen(buf)){
		dprintf(STDOUT_FILENO, "Why won't you tell me that???\n");
		_exit(0);
	}

	dprintf(STDOUT_FILENO, strcmp(flag, buf) ? "You are a liar...\n" : "Thank you, %s!!\n", name);
	return 0;
}

int getaline(char *buf){
        char c = -1;
	int i;

	for(i=0; c && read(STDIN_FILENO, &c, 1); i++){
		if(!(c^'\n'))
			c = '\0';
		buf[i] = c;
	}

	return i;
}
