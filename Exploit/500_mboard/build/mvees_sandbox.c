// gcc -m32 mvees_sandbox.c -o mvees_sandbox

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/syscall.h>

#define true		1
#define false		0

#define RD		0
#define WR		1
#define NR_syscalls	546

typedef struct{
	pid_t pid;
	int status;
	int pipe[3];
	struct user_regs_struct regs;
	int syscall;
	unsigned long long ret;
} PROC_INFO;

void help(char *name);
void set_sys_allow(char *str, char allow_sys[], char permission);

int mvee(int n, char allow_sys[], char *argv[]);
int chk_syscall(PROC_INFO *p_o, PROC_INFO p_r[], char allow_sys[]);
int read_data(PROC_INFO *p_o, PROC_INFO p_r[]);
int write_data(PROC_INFO *p_o, PROC_INFO p_r[]);

pid_t p_run(char* argv[], PROC_INFO *p);
int p_get_syscall(PROC_INFO *p);
int p_do_syscall(PROC_INFO *p);
int p_kill(PROC_INFO *p);

int level, limit;

int main(int argc, char *argv[]){
	char opt;
	int long_opt;
	struct option long_options[] = {
	        {"replicas", required_argument, &long_opt, 'r'},
	        {"allow",    required_argument, &long_opt, 'a'},
	        {"deny",     required_argument, &long_opt, 'd'},
	        {"level",    required_argument, &long_opt, 'l'},
	        {"out-limit",required_argument, &long_opt, 'o'},
	        {"help",     no_argument,       &long_opt, 'h'},
	        {"HIDDEN",   no_argument,       &long_opt, 'H'},
	        {0, 0, 0, 0}};

	char allow_sys_table[NR_syscalls];
	int n_replica = 1, n_level = 1, n_limit = -1;
	static char setted_syscall = false;

	while((opt=getopt_long(argc, argv, "-h", long_options, NULL))!=-1 && opt!=1)
		switch(opt){
			case 0:
				switch(long_opt){
					case 'r':
						n_replica = atoi(optarg);
						break;
					case 'a':
						if(setted_syscall)
							break;
						set_sys_allow(optarg, allow_sys_table, true);
						setted_syscall = true;
						break;
					case 'd':
						if(setted_syscall)
							break;
						set_sys_allow(optarg, allow_sys_table, false);
						setted_syscall = true;
						break;
					case 'l':
						n_level = atoi(optarg);
						break;
					case 'o':
						n_limit = atoi(optarg);
						break;
					case 'H':
						dprintf(STDOUT_FILENO, 	"===================================================\n"
									"Enjoy my simple and powerful mitigation mechanism!!\n"
									"by ShiftCrops\n"
									"===================================================\n"
									"\n");
					case 'h':
						help(argv[0]);
						break;
				}
				break;
			case 'h':
			case ':':
			case '?':
				help(argv[0]);
				break;
		}

	if(!setted_syscall)
		memset(allow_sys_table, true, NR_syscalls);

	level = (n_level >= 0 && n_level < 4) ? n_level : 1;
	limit = n_limit >0 ? n_limit : -1;

	if((optind < argc) || (argc>1 && optind == argc && argv[argc-1][0]^'-'))
		mvee(n_replica, allow_sys_table, argv+optind-1);
	else
		help(argv[0]);
/*
	printf("argc = %d, optind = %d\n", argc, optind);
	for(i=0; argv[i]; i++)
		printf("%s ", argv[i]);
	printf("\n\n");
	if((optind < argc) || (argc>1 && optind == argc && argv[argc-1][0]^'-'))
		printf("argv[0] = %s\n", argv[optind-1]);
*/
}

void help(char *name){
	if(name)
		dprintf(STDOUT_FILENO, 	"Simple MVEEs and Syscall SandBox (for x86)\n"
					"\n"
					"Usage :\n"
					"\t%1$s [options] --allow=Na <program> [args] ...\n"
					"\t%1$s [options] --deny=Nd <program> [args] ...\n"
					"\t%1$s --help\n"
					"\n"
					"Options : \n"
					"\t--replicas=N\t\t\t[Default : 1]\n"
					"\t\tnumber of replicas\n\n"
					"\t--allow=N,...\t\t\t[Default : All]\n"
					"\t\tallow syscall numbers\n\t\tex) --allow=0,1\n\n"
					"\t--deny=N,...\t\t\t[Default : None]\n"
					"\t\tdeny syscall numbers\n\t\tex) --deny=57,59,60\n\n"
					"\t--level=0|1|2|3\t\t\t[Default : 1]\n"
					"\t\tsystemcall check level\n\n"
					"\t--out-limit=N\t\t\t[Default : None]\n"
					"\t\toutput limitation (byte)\n\n"
					"\t--help, -h\n"
					"\t\tshow this help\n"
					"\n", name);

	_exit(0);
}

void set_sys_allow(char *str, char allow_sys[], char permission){
	char *tok;
	char *delim = ",";

	memset(allow_sys, !permission, NR_syscalls);

	dprintf(STDOUT_FILENO, 	"Setting Systemcall Sandbox with %s\n"
				"%s : ", permission ? "Whitelist" : "Blacklist", permission ? "ALLOW" : "DENY");
	for(tok = strtok(str, delim); tok; tok = strtok(NULL, delim)){
		int sys=atoi(tok);

		if(sys>=0 && sys<NR_syscalls){
			allow_sys[sys]=permission;
			dprintf(STDOUT_FILENO, "%d ", sys);
		}
	}
	dprintf(STDOUT_FILENO, "\n\n");
}

int mvee(int n, char allow_sys[], char *argv[]){
	int i, c;
	PROC_INFO orig, *replica;

	dprintf(STDOUT_FILENO,  "Starting Program on Simple MVEEs System\n"
				"Replicas : %d\n"
				"ChkLevel : %s\n"
				"Program  : ", n, level>0 ? level>1 ? "Return Value": "Systemcall Number": "No Check");
	for(i=0; argv[i]; i++)
		dprintf(STDOUT_FILENO, "%s ", argv[i]);
	dprintf(STDOUT_FILENO, 	"\n\n"
				"========================================\n"
				"\n");

	if(!p_run(argv, &orig))
		return -1;

	replica = (PROC_INFO*)malloc(sizeof(PROC_INFO)*(n+1));
	for(i=0; i<n; i++)
		if(!p_run(argv, &replica[i]))
			break;
	replica[n].pid = 0;

	if(i<n){
		for(i--; i>=0; i--)
			p_kill(&replica[i]);
		p_kill(&orig);
		return -1;
	}

	while((c=chk_syscall(&orig, replica, allow_sys)) == n);

	if(!WIFEXITED(orig.status)){
		char *errmsg;

		switch(c){
			case -0x10:
				errmsg = "Error Occurred!!";
				break;
			case -0x20:
				errmsg = "Prohibited Syscall Called!!";
				break;
			case -1:
				errmsg = "Systemcall Responces Different!!";
				break;
			case -2:
				errmsg = "Output Limit Exceeded!!";
				break;
			default:
				errmsg = "Exploitation Detected!!";
		}
		dprintf(STDERR_FILENO, 	"[MVEEs] %s\n"
					"Sending SIGKILL to all processes...\n", errmsg);

		p_kill(&orig);
		for(i=0; i<n; i++)
			p_kill(&replica[i]);

		return 0;
	}

	return 1;
}

int chk_syscall(PROC_INFO *p_o, PROC_INFO p_r[], char allow_sys[]){
	int i, ret = 0;

	if(!p_get_syscall(p_o))
		return -0x10;
	for(i=0; p_r[i].pid; i++)
		if(!p_get_syscall(&p_r[i]) || (level > 0 && p_o->syscall^p_r[i].syscall))
			goto end;

	if(!allow_sys[p_o->syscall])
		return -0x20;

	switch(p_o->syscall){
		case SYS_read:
			ret = read_data(p_o, p_r);
			break;
		case SYS_write:
			ret = write_data(p_o, p_r);
			break;
		default:
			if(!p_do_syscall(p_o))
				return -0x10;
			for(i=0; p_r[i].pid; i++)
				if(!p_do_syscall(&p_r[i]))
				goto end;
	}

end:
	return ret<0 ? ret : i;
}

int read_data(PROC_INFO *p_o, PROC_INFO p_r[]){
	int i, fd;
	size_t count;
	char *buf;

	//fd = p_o->regs.rdi;
	//count = p_o->regs.rdx;
	fd = p_o->regs.ebx;
	count = p_o->regs.edx;
	if(fd==STDIN_FILENO){
		buf = malloc(count);
		count = read(STDIN_FILENO, buf, count);

		write(p_o->pipe[STDIN_FILENO], buf, count);
		ptrace(PTRACE_SINGLESTEP, p_o->pid, NULL, NULL);
		waitpid(p_o->pid, p_o->status, 0);

		ptrace(PTRACE_GETREGS, p_o->pid, NULL, &p_o->regs);
		//p_o->ret = p_o->regs.rax;
		p_o->ret = p_o->regs.eax;

		for(i=0; p_r[i].pid; i++){
			write(p_r[i].pipe[STDIN_FILENO], buf, count);
			ptrace(PTRACE_SINGLESTEP, p_r[i].pid, NULL, NULL);
			waitpid(p_r[i].pid, &p_r[i].status, 0);

			ptrace(PTRACE_GETREGS, p_r[i].pid, NULL, &p_r[i].regs);
			//p_r[i].ret = p_r[i].regs.rax;
			p_r[i].ret = p_r[i].regs.eax;

			if(level > 1 && p_o->ret ^ p_r[i].ret){
				free(buf);
				return -1;
			}
		}

		free(buf);
	}
	else
		count = 0;

	return count;
}

int write_data(PROC_INFO *p_o, PROC_INFO p_r[]){
	int i, fd;
	size_t count;
	char *buf[2];

	//fd = p_o->regs.rdi;
	//count = p_o->regs.rdx;
	fd = p_o->regs.ebx;
	count = p_o->regs.edx;
	if(fd==STDOUT_FILENO || fd==STDERR_FILENO){
		buf[0] = malloc(count*2);
		buf[1] = buf[0]+count;

		ptrace(PTRACE_SINGLESTEP, p_o->pid, NULL, NULL);
		count = read(p_o->pipe[fd], buf[0], count);
		waitpid(p_o->pid, p_o->status, 0);

		ptrace(PTRACE_GETREGS, p_o->pid, NULL, &p_o->regs);
		//p_o->ret = p_o->regs.rax;
		p_o->ret = p_o->regs.eax;

		if(limit > 0){
			if(count > limit)
				count = limit;
			limit -= count;
		}

		for(i=0; p_r[i].pid; i++){
			ptrace(PTRACE_SINGLESTEP, p_r[i].pid, NULL, NULL);
			read(p_r[i].pipe[fd], buf[1], count);
			waitpid(p_r[i].pid, &p_r[i].status, 0);

			ptrace(PTRACE_GETREGS, p_r[i].pid, NULL, &p_r[i].regs);
			//p_r[i].ret = p_r[i].regs.rax;
			p_r[i].ret = p_r[i].regs.eax;

			if((level > 1 && p_o->ret ^ p_r[i].ret) || (level > 2 && memcmp(buf[0], buf[1], count))){
				free(buf[0]);
				return -1;
			}
		}

		write(fd, buf[0], count);
		free(buf[0]);
	}
	else
		count = 0;

	return limit ? count : -2;
}

pid_t p_run(char* argv[], PROC_INFO *p){
	pid_t pid;
	int i, status;
	int pipe_in[2], pipe_out[2], pipe_err[2];

	pipe(pipe_in);
	pipe(pipe_out);
	pipe(pipe_err);

	switch(pid=fork()){
		case 0:
			close(pipe_in[WR]);
			close(pipe_out[RD]);
			close(pipe_err[RD]);

			dup2(pipe_in[RD], STDIN_FILENO);
			dup2(pipe_out[WR], STDOUT_FILENO);
			dup2(pipe_err[WR], STDERR_FILENO);

			for(i=STDERR_FILENO+1; i<0xff; i++)
				close(i);

			if(ptrace(PTRACE_TRACEME, 0, NULL, NULL) == -1){
				perror("ptrace");
				exit(-1);
			}

			execvp(argv[0], argv);
			perror("execvp");
			break;
		case -1:
			perror("fork");
			break;
		default:
			p->pid = pid;

			close(pipe_in[RD]);
			close(pipe_out[WR]);
			close(pipe_err[WR]);

			p->pipe[STDIN_FILENO] = pipe_in[WR];
			p->pipe[STDOUT_FILENO] = pipe_out[RD];
			p->pipe[STDERR_FILENO] = pipe_err[RD];

			waitpid(pid, &status, 0);
			if(WIFEXITED(status) || WIFSIGNALED(status))
				return 0;
	}

	return pid;
}

int p_get_syscall(PROC_INFO *p){
	ptrace(PTRACE_SYSCALL, p->pid, NULL, NULL);
	waitpid(p->pid, &p->status, 0);
	if(WIFEXITED(p->status) || WIFSIGNALED(p->status))
		return 0;

	ptrace(PTRACE_GETREGS, p->pid, NULL, &p->regs);
	//p->syscall=p->regs.orig_rax;
	p->syscall=p->regs.orig_eax;
	if(p->syscall < 0)
		return 0;

	return 1;
}

int p_do_syscall(PROC_INFO *p){
	ptrace(PTRACE_SINGLESTEP, p->pid, NULL, NULL);
	waitpid(p->pid, &p->status, 0);
	if(WIFEXITED(p->status) || WIFSIGNALED(p->status))
		return 0;

	ptrace(PTRACE_GETREGS, p->pid, NULL, &p->regs);
	//p->ret = p->regs.rax;
	p->ret = p->regs.eax;

	return 1;
}

int p_kill(PROC_INFO *p){
	pid_t pid = p->pid;
	int status;

	ptrace(PTRACE_KILL, pid, NULL, NULL);
	waitpid(pid, &status, 0);

	return 1;
}
