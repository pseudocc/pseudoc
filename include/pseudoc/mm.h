#ifndef PSEUDOC_MM_H
#define PSEUDOC_MM_H

#include <pseudoc/pptr.h>

typedef struct memory_s memory_t;

/**
 * garbage collect
 * 
 * Try to merge the adjacent free pptr_t,
 * and update the free memory pool.
 **/
static void pcollect(memory_t* mp);

static void pmemcpy(pptr_t* dst, pptr_t* src);
static inline void try_cc(memory_t* mp);

/**
 * memory_t constructor
 **/
memory_t* pminit();

/**
 * memory allocate
 **/
pptr_t* pmalloc(memory_t* mp, size_t n_bytes);

/**
 * memory re-allocate
 **/
pptr_t* prealloc(memory_t* mp, pptr_t* p, size_t n_bytes);

/**
 * mark certain pptr_t as free
 **/
void pfree(memory_t* mp, pptr_t* p);

/**
 * memory_t deconstructor
 **/
void pmclean(memory_t* mp);

#endif
