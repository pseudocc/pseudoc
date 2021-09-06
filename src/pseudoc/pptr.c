#include <pseudoc/pptr.h>
#include <stdlib.h>

#define ALIGN_NBITS 2

static const unsigned ALIGN_MASK = (1 << ALIGN_NBITS) - 1;
static const unsigned MAX_PPTR_SIZE = 0x7ffffffc;

pptr_t* pptr(byte_t* head, unsigned offset, unsigned n_bytes) {
  if (n_bytes > MAX_PPTR_SIZE)
    return NULL;
  if (n_bytes & ALIGN_MASK)
    n_bytes = ((n_bytes >> ALIGN_NBITS) + 1) << ALIGN_NBITS;
  
  pptr_t* p = (pptr_t*)(head + offset);
  p->offset = offset;
  p->size = n_bytes;
  p->free = true;

  return p;
}

inline byte_t* pptr_cptr(pptr_t* p) {
  return (byte_t*)++p;
}
