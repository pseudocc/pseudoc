#ifndef PSEUDOC_SALLOC_H
#define PSEUDOC_SALLOC_H

#include <mm/addr.h>

caddr_t stack_alloc(uint32_t n_bytes);
void stack_dealloc();

#endif
