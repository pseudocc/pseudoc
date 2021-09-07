#ifndef PSEUDOC_PPTR_H
#define PSEUDOC_PPTR_H

#include <pseudoc/types.h>

struct pptr_s {
  /**
   * distance to the head
   **/
  unsigned offset;
  unsigned free: 1;
  unsigned size: 31;
};

/**
 * pptr_t constructor
 * 
 * Due to memory alignment, size of the pointer might not be equal to n_bytes.
 **/
pptr_t* pptr(byte_t* head, unsigned offset, unsigned n_bytes);

/**
 * cut pptr_t into 2 pptr_t
 * 
 * When return value is not NULL, total size of
 * the 2 pointers is actually reduced by sizeof(pptr_t).
 * 
 * l_size represent the size of the left one.
 * return NULL when cannot perform the cut.
 **/
pptr_t* pptr_cut(pptr_t* p, unsigned l_size);

/**
 * get head position of this memory region
 **/
inline byte_t* pptr_head(pptr_t* p);

/**
 * get the pptr_t next to p
 **/
inline pptr_t* pptr_next(pptr_t* p);

/**
 * convert pptr_t to c pointer.
 **/
inline byte_t* pptr_cptr(pptr_t* p);

#endif
