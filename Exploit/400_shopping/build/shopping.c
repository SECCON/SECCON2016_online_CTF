// gcc -m32 shopping.c -o shopping
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define BUF_SIZE 64
#define INIT_MONEY 1000000

typedef struct PRODUCT{
	char *name;
	unsigned int price;
	float prof_rate;
	unsigned int stock;
	struct PRODUCT *next;
} Product;

typedef struct CART{
	Product *p;
	unsigned int amount;
	struct CART *next;
} Cart;

typedef struct REPORT{
	char *name;
	char reason[BUF_SIZE];
} Report;

int m_shop;
Report *report;
Product *l_product;
Cart *l_cart;

__attribute__((constructor))
init(){
	srand(time(NULL));
	setbuf(stdin, NULL);
	setbuf(stdout, NULL);
	setbuf(stderr, NULL);

	m_shop = INIT_MONEY;
	report = NULL;
	l_product = NULL;
	l_cart = NULL;
}

void shop(void);
void add_product(void);
void list_product(void);
Product *get_product(char *name);
void free_product(void);

void customer(void);
void add_cart(void);
unsigned int list_cart(void);
Cart *get_cart(Product *p);
void buy_cart(void);
void free_cart(void);

void send_report(void);
void show_edit_report(void);

float rand_range(float s, float e);
int getnline(char *buf, int len);
int getint(void);

int main(void){
	int menu;

	do{
		fprintf(stdout, "\n"
				"== SHOPPING ==\n"
				"1. Shop mode\n"
				"2. Customer mode\n"
				"0. Exit\n"
				": ");
		menu = getint();
		switch(menu){
			case 0:
				break;
			case 1:
				shop();
				break;
			case 2:
				customer();
				break;
			case -1:
				show_edit_report();
				break;
			default:
				fprintf(stderr, "Wrong Input...\n");
		}
	}while(menu);
}

void shop(void){
	int menu;

	if(m_shop < 0){
	//if(m_shop > INIT_MONEY){
		char buf[4];

		fprintf(stderr, "WTF!? My shop went bankrupt...\n"
				"Can you cooperate with the bug report? (y/N) >> ");
		getnline(buf, sizeof(buf));
		if(buf[0]=='y'||buf[0]=='Y')
			send_report();

		m_shop = INIT_MONEY;
		free_cart();
		free_product();
	}

	do{
		fprintf(stdout, "\n"
				"#### SHOP MODE ($%u) ####\n"
				"1. Add Product to list\n"
				"2. List Product\n"
				"3. Reset Product list\n"
				"0. Return\n"
				": ", m_shop);
		menu = getint();
		switch(menu){
			case 0:
				break;
			case 1:
				add_product();
				break;
			case 2:
				list_product();
				break;
			case 3:
				free_cart();
				free_product();
				break;
			default:
				fprintf(stderr, "Wrong Input...\n");
		}
	}while(menu);
}

void add_product(void){
	Product *p;
	char buf[BUF_SIZE*2];
	int n;

	fprintf(stdout, "Name >> ");
	getnline(buf, sizeof(buf));

	if(!(p = get_product(buf))){
		fprintf(stdout, "Price >> ");
		n = getint();
		if(n<0) return;

		p = (Product *)malloc(sizeof(Product));
		if(!p) exit(0);

		p->name = strdup(buf);
		p->price = n;
		p->prof_rate = rand_range(0.75, 1.75);
		p->stock = 0;
		p->next = l_product;
		l_product = p;
	}

	fprintf(stdout, "Stock >> ");
	n = getint();
	if(n<0) return;

	if(p->price * n > m_shop){
		n = m_shop/p->price;
		fprintf(stderr, "Shortage of funds\n");
	}

	m_shop -= p->price * n;
	p->stock += n;

	fprintf(stdout, "ADD PRODUCT SUCCESS\n");
}

void list_product(void){
	Product *p;
	int i;

	fprintf(stdout, "\n"
			"&&&&&& PRODUCT &&&&&&\n");
	for(p=l_product, i=1; p; p=p->next, i++)
		fprintf(stdout, "%03d : %s($%u x %.2f) x %d\n", i, p->name, p->price, p->prof_rate, p->stock);
	fprintf(stdout, "LIST DONE\n");
}

Product *get_product(char *name){
	Product *p;

	for(p=l_product; p; p=p->next)
		if(!strcmp(name, p->name))
			break;
	return p;
}

void free_product(void){
	Product *p, *next;

	for(p=l_product; p; p=next){
		next = p->next;
		free(p->name);
		free(p);
	}

	l_product = NULL;
}

void customer(void){
	int menu;
	unsigned int sum;

	do{
		fprintf(stdout, "\n"
				"#### CUSTOMER MODE ####\n"
				"1. Add to Cart\n"
				"2. List Cart\n"
				"3. Buy\n"
				"4. Reset Cart\n"
				"0. Return\n"
				": ");
		menu = getint();
		switch(menu){
			case 0:
				break;
			case 1:
				add_cart();
				break;
			case 2:
				list_cart();
				break;
			case 3:
				buy_cart();
			case 4:
				free_cart();
				break;
			default:
				fprintf(stderr, "Wrong Input...\n");
		}
	}while(menu);
}

void add_cart(void){
	Product *p;
	Cart *c;
	char buf[BUF_SIZE*2];
	int amount;

	fprintf(stdout, "Product name >> ");
	getnline(buf, sizeof(buf));
	if(!(p = get_product(buf))){
		fprintf(stderr, "Not exists.\n");
		return;
	}

	fprintf(stdout, "Amount >> ");
	amount = getint();

	if(!(c = get_cart(p))){
		c = (Cart *)malloc(sizeof(Cart));
		if(!c) exit(0);

		c->p = p;
		c->amount = 0;
		c->next = l_cart;
		l_cart = c;
	}

	if(amount<0 && -amount > c->amount){
		c->amount = 0;
		fprintf(stderr, "Amount can not be negative\n");
	}
	else{
		if (c->amount+amount > c->p->stock){
			c->amount = c->p->stock;
			fprintf(stderr, "Shortage of stock\n");
		}
		else
			c->amount += amount;

		fprintf(stdout, "ADD TO CART SUCCESS\n");
	}
}

unsigned int list_cart(void){
	Cart *c;
	unsigned int sum, price;
	int i;

	fprintf(stdout, "\n"
			"$$$$$$ CART $$$$$$\n");
	for(c=l_cart, sum=0, i=1; c; c=c->next, i++){
		Product *p = c->p;
		if(!c->amount)
			continue;

		price =  (unsigned int)(p->price * p->prof_rate);
		fprintf(stdout, "%03d : %s($%u) x %d\n", i, p->name, price, c->amount);
		sum += price * c->amount;
	}
	fprintf(stdout, "Total: $%u\n", sum);

	return sum;
}

Cart *get_cart(Product *p){
	Cart *c;

	for(c=l_cart; c; c=c->next)
		if(c->p == p)
			break;
	return c;
}

void buy_cart(void){
	Cart *c;
	unsigned int sum;

	for(c=l_cart, sum=0; c; c=c->next){
		Product *p = c->p;

		p->stock -= c->amount;
		sum += (unsigned int)(p->price * p->prof_rate) * c->amount;
	}
	m_shop += sum;

	fprintf(stdout, "Total: $%u\n"
			"Thank you for your purchase.\n", sum);
}

void free_cart(void){
	Cart *c, *next;

	for(c=l_cart; c; c=next){
		next = c->next;
		free(c);
	}

	l_cart = NULL;
}

void send_report(void){
	time_t timer;
	struct tm *local;
	char buf[BUF_SIZE];

	if(!report)
		report = (Report *)calloc(1, sizeof(Report));
	if(!report) exit(0);

	fprintf(stdout, "\n"
			"#$&#$&#$& SEND BUG REPORT &$#&$#&$#\n");
	if(!report->name){
		fprintf(stdout, "your name  : ");
		getnline(buf, sizeof(buf));
		report->name = strdup(buf);
	}

	fprintf(stdout, "when crash : ");
	getnline(buf, sizeof(buf));

	timer = time(NULL);
	local = localtime(&timer);
	snprintf(report->reason, sizeof(report->reason), "[%4d/%02d/%02d %02d:%02d:%02d] "
				 , local->tm_year+1900, local->tm_mon+1, local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec);
	strncat(report->reason, buf, sizeof(report->reason));

	fprintf(stdout, "Thank you for sending me a bug report\n");
}

void show_edit_report(void){
	char buf[BUF_SIZE];
	fprintf(stdout,	"\n"
			"#$&#$&#$& SHOW BUG REPORT &$#&$#&$#\n");
	if(!report){
		fprintf(stderr, "No report exists\n");
		return;
	}

	fprintf(stdout, "%s\n(%s)\n", report->reason, report->name);

	fprintf(stdout, "Change name? (y/N) >> ");
	getnline(buf, sizeof(buf));
	if(buf[0]=='y'||buf[0]=='Y'){
		fprintf(stdout, "your name  : ");
		getnline(report->name, malloc_usable_size(report->name));
	}

	fprintf(stdout, "Change reason? (y/N) >> ");
	getnline(buf, sizeof(buf));
	if(buf[0]=='y'||buf[0]=='Y'){
		fprintf(stdout, "when crash : ");
		getnline(report->reason, sizeof(report->reason));
	}
}

float rand_range(float s, float e){
	int v;

	if(e < s)
		return 0;

	v = rand();
	v %= (int)((e-s)*100);

	return s+v/100.0;
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
