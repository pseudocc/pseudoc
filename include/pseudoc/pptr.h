#ifndef PSEUDOC_PPTR_H
#define PSEUDOC_PPTR_H

#include <pseudoc/types.h>

struct pptr_s {
  unsigned offset;
  unsigned free: 1;
  unsigned size: 31;
};

pptr_t* pptr(byte_t* head, unsigned offset, unsigned n_bytes);
pptr_t* pptr_cut(pptr_t* p, unsigned l_size);

inline byte_t* pptr_head(pptr_t* p);
inline pptr_t* pptr_next(pptr_t* p);
inline byte_t* pptr_cptr(pptr_t* p);

#endif
