// gcc -m32 -pie -fPIE mboard.c -o mboard

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>

#define true	1
#define false	0

#define NAME_SIZE 0x10
#define BUF_SIZE 0x100

typedef struct {
	char name[NAME_SIZE];
	char *msg;
} Entry;

typedef struct {
	char exist;
	Entry *entry;
} List;

List *list = NULL;
int list_size = 0x10;

void deley(long int milli_sec);
int service(char **pformat);
void chg_format(char **pformat);

int list_entry(char *format);
int register_entry(void);
int remove_entry(int id);
int modify_message(int id);

int getnline(char *buf, int len);
int getint(void);

int main(void){
	char *format __attribute__((aligned(0x100)));

	format = strdup("%02d | %-16s | %s\n");
	if(!(list = (List*)calloc(list_size, sizeof(List))))
		exit(0);

	dprintf(STDOUT_FILENO, "Message Board Service\n");
	while(service(&format));

	free(format);
}

void deley(long int milli_sec){
	struct timespec req = {0, milli_sec * 1000000};

	if(nanosleep(&req, NULL)){
		perror("nanosleep");
		exit(0);
	}
}

int service(char **pformat){
	int menu, id;

	dprintf(STDOUT_FILENO, 	"\n"
				"1 : List\n"
				"2 : Register\t3 : Remove\t4 : Modify\n"
				"5 : Change List Format\n"
				"0 : Exit\n"
				"menu > ");
	menu = getint();
	dprintf(STDOUT_FILENO, "\n");

	switch(menu){
		case 5:
			chg_format(pformat);
			break;
		case 4:
		case 3:
			dprintf(STDOUT_FILENO, "%s\nid   >> ", menu==3 ? "Remove Entry": "Modify Message");
			id = getint();
		case 2:
			deley(50);
			if(menu > 2 ? menu > 3 ? modify_message(id) : remove_entry(id) : register_entry())
				dprintf(STDOUT_FILENO, "[+] Success!\n");
			else
				dprintf(STDERR_FILENO, "[-] Failure...\n");
			break;
		case 1:
			dprintf(STDOUT_FILENO, "Message Board Entry\n");
			dprintf(STDOUT_FILENO, "%d entries exist\n", list_entry(*pformat));
			break;
		case 0:
			break;
		default:
			dprintf(STDERR_FILENO, "[!] Wrong Input...\n");
	}

	return menu;
}

void chg_format(char **pformat){
	char buf[BUF_SIZE]={0}, *f, *n;
	char delim;

	dprintf(STDOUT_FILENO, "Change List Format\n");

	dprintf(STDOUT_FILENO, "Delimiter >> ");
	getnline(buf, sizeof(buf));
	if(!((delim = buf[0])^'%'))
		return;

	dprintf(STDOUT_FILENO, "Print id? (Y/n) >> ");
	getnline(buf, sizeof(buf));
	f = buf[0]=='n' ? "%3$s %1$c %4$s\n" : "%2$s %1$c %3$s %1$c %4$s\n";

	dprintf(STDOUT_FILENO, "Name align? (Y/n) >> ");
	getnline(buf, sizeof(buf));
	n = buf[0]=='n' ? "%2$s" : "%2$-16s";

	snprintf(buf, sizeof(buf), f, delim, "%1$02d", n, "%3$s");
	free(*pformat);
	*pformat = strdup(buf);
}

int list_entry(char *format){
	int i, c;

	for(i=c=0; i<list_size; i++)
		if(list[i].exist){
			dprintf(STDOUT_FILENO, format, i, list[i].entry->name, list[i].entry->msg);
			c++;
		}

	return c;
}

int register_entry(void){
	int i, len;

	dprintf(STDOUT_FILENO, "Register Entry\n");
	for(i=0; i<list_size; i++)
		if(!list[i].exist)
			break;

	if(i == list_size){
		dprintf(STDERR_FILENO, "[*] extend list\n");
		list_size *= 2;
		if(!(list = (List*)realloc(list, sizeof(List)*list_size)))
			exit(0);
		memset(list+list_size/2, 0, sizeof(List)*list_size/2);
	}

	if(!(list[i].entry = (Entry*)malloc(sizeof(Entry))))
		return 0;

	dprintf(STDOUT_FILENO, "name >> ");
	getnline(list[i].entry->name, NAME_SIZE);

	dprintf(STDOUT_FILENO, "len  >> ");
	len = getint();

	if(!len || !(list[i].entry->msg = (char*)malloc(sizeof(char)*len))){
		free(list[i].entry);
		return 0;
	}
	dprintf(STDOUT_FILENO, "msg  >> ");
	getnline(list[i].entry->msg, len);

	list[i].exist = true;

	return 1;
}

int remove_entry(int id){
	if(id < 0 || id >= list_size)
		return 0;

	if(!list[id].entry)
		return 0;

	list[id].exist = false;
	free(list[id].entry->msg);
	free(list[id].entry);

	return 1;
}

int modify_message(int id){
	int len;

	if(id < 0 || id >= list_size)
		return 0;

	if(!list[id].exist || !list[id].entry)
		return 0;

	dprintf(STDOUT_FILENO, "len  >> ");
	if(!(len = getint()))
		return 0;

	if(!(list[id].entry->msg = (char*)realloc(list[id].entry->msg, sizeof(char)*len))){
		remove_entry(id);
		return 0;
	}

	dprintf(STDOUT_FILENO, "msg  >> ");
	getnline(list[id].entry->msg, len);

	return 1;
}

int getnline(char *buf, int len){
	char *lf;

	read(STDIN_FILENO, buf, len);
	if(lf=strchr(buf, '\n'))
		*lf='\0';

	return strlen(buf);
}

int getint(void){
	char buf[BUF_SIZE]={0};

	if(!getnline(buf, sizeof(buf)))
		return 0;

	return atoi(buf);
}
