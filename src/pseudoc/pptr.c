#include <pseudoc/pptr.h>
#include <stdlib.h>

#define ALIGN_NBITS 2

static const unsigned CUT_MIN = (1 << ALIGN_NBITS) + sizeof(pptr_t);
static const unsigned ALIGN_MASK = (1 << ALIGN_NBITS) - 1;
static const unsigned MAX_PPTR_SIZE = 0x7ffffffc;

#define ALIGN_BYTES(n_bytes)  \
  if (n_bytes & ALIGN_MASK)   \
    n_bytes = ((n_bytes >> ALIGN_NBITS) + 1) << ALIGN_NBITS

pptr_t* pptr(byte_t* head, unsigned offset, unsigned n_bytes) {
  if (n_bytes > MAX_PPTR_SIZE)
    return NULL;
  ALIGN_BYTES(n_bytes);
  
  pptr_t* p = (pptr_t*)(head + offset);
  p->offset = offset;
  p->size = n_bytes;
  p->free = true;

  return p;
}

pptr_t* pptr_cut(pptr_t* p, unsigned l_size) {
  ALIGN_BYTES(l_size);
  if (p->size - l_size > CUT_MIN)
    return pptr(pptr_head(p), p->offset + l_size, 
      p->size - l_size - sizeof(pptr_t));
  return NULL;
}

inline byte_t* pptr_head(pptr_t* p) {
  return (byte_t*)p - p->offset;
}

inline byte_t* pptr_next(pptr_t* p) {
  return (byte_t*)p + p->size;
}

inline byte_t* pptr_cptr(pptr_t* p) {
  return (byte_t*)++p;
}
