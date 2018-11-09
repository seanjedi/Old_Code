#include <stdio.h>

void *latest_mmap_addr; // make the address returned by mmap accessible

void *__real_mmap(void *addr, size_t len, int prot, int flags, int fildes,
                  off_t off);
void *__wrap_mmap(void *addr, size_t len, int prot, int flags, int fildes,
                  off_t off)
{
    latest_mmap_addr = __real_mmap(addr, len, prot, flags, fildes, off);
    return latest_mmap_addr;
}
