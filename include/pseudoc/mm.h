#ifndef PSEUDOC_MM_H
#define PSEUDOC_MM_H

#include <pseudoc/pptr.h>

typedef struct memory_s memory_t;

static void pcollect(memory_t* mp);
static void pmemcpy(pptr_t* dst, pptr_t* src);
static inline void try_cc(memory_t* mp);

memory_t* pminit();
pptr_t* pmalloc(memory_t* mp, size_t n_bytes);
pptr_t* prealloc(memory_t* mp, pptr_t* p, size_t n_bytes);

void pfree(memory_t* mp, pptr_t* p);
void pmclean(memory_t* mp);

#endif
