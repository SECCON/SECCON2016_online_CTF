#include "encrypt.h"
#include "decrypt.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

void usage() {
  puts("cipher (encrypt|decrypt) key input output");
  exit(-1);
}
int main(int argc, char** argv) {
  void (*func)(unsigned char *,unsigned char *);
  int enc = 1;
  if(argc != 5) {
    usage();
  }else if(strcmp(argv[1], "encrypt") == 0) {
    func = encrypt;
    enc = 1;
  }else if(strcmp(argv[1], "decrypt") == 0) {
    func = decrypt;
    enc = 0;
  }else{
    usage();
  }
  FILE *fk = fopen(argv[2], "r");
  FILE *fr = fopen(argv[3], "r");
  FILE *fw = fopen(argv[4], "w");
  unsigned char key[16];
  unsigned char data[16] = {};
  unsigned int size = 0;
  fread(key, 1, 16, fk);
  fclose(fk);
  int len = 0;
  for(;;) {
    int len = fread(data, 1, 16, fr);
    if(len == 0 && !enc) break;
    size += 16;
    for(int i = len; i < 16; i++) {
      data[i] = 16 - len;
    }
    func(data, key);
    fwrite(data, 1, 16, fw);
    if(len < 16) break;
  }
  size -= data[15];
  fclose(fr);
  fclose(fw);
  if(!enc) {
    truncate(argv[4], size);
  }
}
