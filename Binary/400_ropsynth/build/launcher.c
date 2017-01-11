/* vim:set ai sw=4 ts=4 et: */
#include <stdio.h>
#include <sys/mman.h>
#include "seccomp-bpf.h"

#define PAYLOAD_LENGTH 4096

extern char __executable_start[];
extern char _end[];
void get_self(void **paddr, size_t *plen)
{
    *paddr = (void*)__executable_start;
    *plen = (size_t)(_end - __executable_start);
}

int restrict_syscalls()
{
    struct sock_filter filter[] = {
        VALIDATE_ARCHITECTURE,
        EXAMINE_SYSCALL,
        ALLOW_SYSCALL(open),
        ALLOW_SYSCALL(read),
        ALLOW_SYSCALL(write),
        ALLOW_SYSCALL(close),
        ALLOW_SYSCALL(munmap),
        ALLOW_SYSCALL(exit_group),
        KILL_PROCESS,
    };
    struct sock_fprog fprog = {
        sizeof(filter) / sizeof(filter[0]),
        filter,
    };

    if(prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0) < 0){
        return -1;
    }

    if(prctl(PR_SET_SECCOMP, SECCOMP_MODE_FILTER, &fprog) < 0){
        return -1;
    }

    return 0;
}

typedef void (*UNMAP_SELF)(void *ptr, size_t len, char *ropchain);
__asm__(
"unmap_self:"// rdi: ptr, rsi: len, rdx: ropchain
    "push %rdx;"                        // ropchain
    "mov  $11, %rax;"                   // munmap
    "syscall;"
    "mov  $11, %rax;"                   // munmap
    "lea  (%rip), %rdi;"
    "pop  %rsp;"                        // ropchain
    "and  $0xfffffffffffff000, %rdi;"   // ptr
    "mov  $0x1000, %rsi;"               // len
    "syscall;"
"sizeof_unmap_self = .-unmap_self;"
    "ret;"
);

extern char unmap_self[];
extern char sizeof_unmap_self[];

void start_rop(char *ropchain)
{
    void        *addr;
    size_t      len;
    char        *trampoline;
    UNMAP_SELF  entry;

    get_self(&addr, &len);
    trampoline = mmap(NULL, getpagesize() * 2, PROT_READ|PROT_WRITE|PROT_EXEC,
        MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    entry = (UNMAP_SELF)(
        trampoline + getpagesize() - (size_t)sizeof_unmap_self);
    memcpy(entry, (void*)unmap_self,
        (size_t)sizeof_unmap_self + 1/* for ret */);
    mprotect(trampoline, getpagesize() * 2, PROT_READ|PROT_EXEC);
    if(restrict_syscalls() < 0) return;

    entry(addr, len, ropchain);
    return;
}

int main(int argc, char *argv[])
{
    char *gadgets;
    char ropchain[4096];
    char *data;

    // 0x00800000: gadgets
    // 0x00a00000: data region
    gadgets = mmap((void*)0x00800000, 4096, PROT_READ|PROT_WRITE,
        MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    fread(gadgets, 1, 4096, stdin);
    mprotect(gadgets, 4096, PROT_READ|PROT_EXEC);
    fread(ropchain, 1, sizeof(ropchain), stdin);
    data = mmap((void*)0x00a00000, 4096, PROT_READ|PROT_WRITE,
        MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    strcpy(data, "secret");
    if(argc >= 2){
        alarm(10);
        if(chroot(argv[1]) < 0) return -1;
        if(chdir("/") < 0) return -1;
        if(setgid(31337) < 0) return -1;
        if(setuid(31337) < 0) return -1;
    }
    start_rop(ropchain);

    return -1;
}
