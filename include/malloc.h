#ifndef PSEUDOC_MALLOC_H
#define PSEUDOC_MALLOC_H

#include <mm/addr.h>

paddr_t malloc(uint32_t n_bytes);
void free(paddr_t addr);

#endif
