/**
 * memory management
 * 
 * Free a pptr_t will make the chunk pool dirty,
 * so we run pcollect regularly.
 **/

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
static inline void try_rc(memory_t* mp);

/**
 * memory_t constructor
 * 
 * n_bytes is the size of quick memory region
 **/
memory_t* pminit(unsigned n_bytes);

/**
 * memory allocate
 **/
pptr_t* pmalloc(memory_t* mp, unsigned n_bytes);

/**
 * memory re-allocate
 **/
pptr_t* prealloc(memory_t* mp, pptr_t* p, unsigned n_bytes);

/**
 * mark certain pptr_t as free
 * 
 * Does not merge with adjacent free pptr_t.
 **/
void pfree(memory_t* mp, pptr_t* p);

/**
 * memory_t deconstructor
 **/
void pmclean(memory_t* mp);

void* qget(memory_t* mp, unsigned n_bytes);
void qpop(memory_t* mp, unsigned n_bytes);
void qreset(memory_t* mp);

#endif
