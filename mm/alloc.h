#ifndef PSEUDOC_ALLOC_H
#define PSEUDOC_ALLOC_H

#include <mm/addr.h>
#include <malloc.h>
#include <salloc.h>

int32_t stack_init(uint32_t size);
inline caddr_t std_malloc(uint64_t size);
static void memory_sort();

#endif
