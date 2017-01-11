// gcc chat.c -o chat
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 128
#define MAX_NAME 32

struct user{
	char *name;

	struct tweet{
		int id;
		struct user *user;
		char msg[BUFSIZE];
		struct tweet *next;
	} *dm;
	struct user *next;
};

typedef struct user USER;
typedef struct tweet TWEET;

USER *user_tbl['z'-'a'+2] = {NULL};
TWEET *tl = NULL;
int tweet_count = 0;

int login(USER **user, char *name);
void logout(USER **user);

void service(USER *user);
void post_tweet(USER *from, USER *to, char *msg);
int get_tweet(USER *user);

int signup(char *name);
int change_name(USER *target, char *name);
void remove_user(USER *user);
USER *get_user(char *name);
int hash(char *str);
void list_users(void);

int getnline(char *buf, int len);
int getint(void);

__attribute__((constructor))
init(){
	setbuf(stdin, NULL);
	setbuf(stdout, NULL);
	setbuf(stderr, NULL);
}

int main(void){
	USER *login_user = NULL;
	char buf[BUFSIZE];
	int menu, result;

	fprintf(stdout, "Simple Chat Service\n");
	do{
		if(login_user){
			service(login_user);
			logout(&login_user);
		}

		fprintf(stdout, "\n"
				"1 : Sign Up\t2 : Sign In\n"
				"0 : Exit\n"
				"menu > ");
		switch(menu = getint()){
			case 1:
			case 2:
				fprintf(stdout, "name > ");
				getnline(buf, MAX_NAME);
				result = menu==1 ? signup(buf) : login(&login_user, buf);
				if(result==1)
					fprintf(stdout, "Success!\n");
				else
					fprintf(stderr, "Failure...\n");
			case 0:
				break;
			default:
				fprintf(stderr, "Wrong Input...\n");
		}
	}while(menu);
	fprintf(stdout, "Thank you for using Simple Chat Service!\n");
}

int login(USER **user, char *name){
	*user = get_user(name);

	if(!*user){
		fprintf(stderr, "User '%s' does not exist.\n", name);
		return 0;
	}

	fprintf(stdout, "Hello, %s!\n", name);
	return 1;
}

void logout(USER **user){
	if(*user)
		fprintf(stdout, "Bye, %s\n", (*user)->name);
	*user = NULL;
}

/********************************/

void service(USER *user){
	USER *target;
	char buf[BUFSIZE];
	int menu;

	fprintf(stdout, "\nService Menu\n");
	do{
		fprintf(stdout, "\n"
				"1 : Show TimeLine\t2 : Show DM\t3 : Show UsersList\n"
				"4 : Send PublicMessage\t5 : Send DirectMessage\n"
				"6 : Remove PublicMessage\t\t7 : Change UserName\n"
				"0 : Sign Out\n"
				"menu >> ");
		switch(menu = getint()){
			case 0:
				break;
			case 1:
				get_tweet(NULL);
				break;
			case 2:
				get_tweet(user);
				break;
			case 3:
				list_users();
				break;
			case 4:
				fprintf(stdout, "message >> ");
				getnline(buf, BUFSIZE);
				post_tweet(user, NULL, buf);
				break;
			case 5:
				fprintf(stdout, "name >> ");
				getnline(buf, MAX_NAME);
				target = get_user(buf);

				if(target){
					fprintf(stdout, "message >> ");
					getnline(buf, BUFSIZE);
					post_tweet(user, target, buf);
				}
				else
					fprintf(stderr, "User '%s' does not exist.\n", buf);
				break;
			case 6:
				fprintf(stdout, "id >> ");
				switch(remove_tweet(user, getint())){
					case 0:
						fprintf(stderr, "Message not found.\n");
						break;
					case -1:
						fprintf(stderr, "Can not remove other user's message.\n");
						break;
				}
				break;
			case 7:
				fprintf(stdout, "name >> ");
				getnline(buf, MAX_NAME);
				if(change_name(user, buf)<0)
					menu = 0;
				break;
			default:
				fprintf(stderr, "Wrong Input...\n");
		}
		if(menu)
			fprintf(stdout, "Done.\n");
	}while(menu);
}

void post_tweet(USER *from, USER *to, char *msg){
	TWEET *tweet = (TWEET*)malloc(sizeof(TWEET));

	tweet->user = from;
	linecpy(tweet->msg, msg, sizeof(tweet->msg));

	if(to){
		tweet->id = 0;
		tweet->next = to->dm;
		to->dm = tweet;
	}
	else{
		tweet->id = ++tweet_count;
		tweet->next = tl;
		tl = tweet;
	}
}

int get_tweet(USER *user){
	TWEET *tweet;
	int i;
	char *format;

	fprintf(stdout, user ? "Direct Messages\n" : "Time Line\n");
	format = user ? "[%s] %s\n" : "(%3$03d)[%s] %s\n";

	for(i = 0, tweet = user ? user->dm : tl; tweet; tweet = tweet->next, i++)
		fprintf(stdout, format, tweet->user->name, tweet->msg, tweet->id);

	return i;
}

int remove_tweet(USER *user, int id){
	TWEET *tweet;

	for(tweet=tl; tweet&&tweet->id!=id; tweet=tweet->next);
	if(!tweet)
		return 0;
	if(tweet->user!=user)
		return -1;

	if(tweet==tl){
		tl = tweet->next;
		free(tweet);
	}
	else{
		TWEET *target = tweet;
		for(tweet=tl; tweet&&tweet->next!=target; tweet=tweet->next);
		tweet->next = target->next;
		free(target);
	}	
	return 1;
}

/********************************/

int signup(char *name){
	USER *user;
	int idx;

	if(get_user(name)){
		fprintf(stderr, "User '%s' already exists\n", name);
		return 0;
	}

	user = (USER*)malloc(sizeof(USER));
	if((idx = hash(name))<0){
		free(user);
		fprintf(stderr, "Signup failed...\n");
		return -1;
	}
	user->name = strdup(name);
	user->dm = NULL;

	user->next = user_tbl[idx];
	user_tbl[idx] = user;

	return 1;
}

int change_name(USER *target, char *name){
	USER *user;
	int idx;

	if((idx = hash(target->name))<0)
		return -1;
	if(get_user(name)){
		fprintf(stderr, "User '%s' already exists\n", name);
		return 0;
	}

	if(user_tbl[idx] == target)
		user_tbl[idx] = target->next;
	else{
		for(user = user_tbl[idx]; user && user->next != target; user = user->next);
		if(!user)
			return -1;
		user->next = target->next;
	}

	// vuln
	linecpy(target->name, name, MAX_NAME);
	if((idx = hash(name))<0){
		fprintf(stderr, "Change name error...\n");
		remove_user(target);
		return -1;
	}
/*
	free(target->name);
	if((idx = hash(name))<0){
		fprintf(stderr, "Change name error...\n");
		free(target);
		return -1;
	}
	target->name = strdup(name);
*/

	target->next = user_tbl[idx];
	user_tbl[idx] = target;

	return 1;
}

void remove_user(USER *user){
	TWEET *tweet, *dm, *next;

	for(dm=user->dm; dm; dm=next){
		next = dm->next;
		free(dm);
	}

	for(tweet=tl; tweet; tweet=tweet->next)
		if(tweet->next && tweet->next->user==user){
			next = tweet->next;
			tweet->next = next->next;
			free(next);
		}
	if(tl && tl->user==user){
		next = tl;
		tl = next->next;
		free(next);
	}

	free(user->name);
	free(user);
}

USER *get_user(char *name){
	USER *user;
	int idx;

	if((idx = hash(name))>=0)
		for(user = user_tbl[idx]; user; user = user->next)
			if(!strcmp(user->name, name))
				return user;

	return NULL;
}

int hash(char *str){
	int headc;

	if(!str)
		return -1;

	headc = tolower(str[0]);
	if(!isprint(headc))
		return -1;
	if(headc<'a' || headc>'z')
		return 0;
	return headc-'a'+1;
}

void list_users(void){
	USER *user;
	int i;

	fprintf(stdout, "Users List\n");
	for(i=0; i<sizeof(user_tbl)/sizeof(USER*); i++)
		for(user = user_tbl[i]; user; user = user->next)
			fprintf(stdout, "* %s\n", user->name);
}

/********************************/

int getnline(char *buf, int len){
        char *lf;

        fgets(buf, len, stdin);
        if(lf=strchr(buf, '\n'))
                *lf='\0';

        return strlen(buf);
}

int getint(void){
	char buf[BUFSIZE]={0};

	if(!getnline(buf, sizeof(buf)))
		return 0;
	return atoi(buf);
}

int linecpy(char *dst, char *src, int len){
	int i;

	for(i=0; i<len; i++){
		dst[i]=src[i];
		if(!(src[i] && src[i]^'\n'))
			break;
	}

	return i;
}
