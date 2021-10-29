#ifndef PSEUDOC_MALLOC_H
#define PSEUDOC_MALLOC_H

#include <malloc/addr.h>

paddr_t malloc(int32_t n_bytes);
paddr_t realloc(paddr_t addr, int32_t n_bytes);
void free(paddr_t addr);

#endif
