#include <stdlib.h>
#include <string.h>
#include <pseudoc/mm.h>

#define CC_FREQ 1000
#define N_CHUNKS 8
struct memory_s {
  unsigned cc_count_down;

  unsigned n_chunks;
  unsigned n_chunks_limits;
  mchunk_t** chunks;
};

typedef struct pptr_list_s pptr_list_t;
struct pptr_list_s {
  pptr_t* ptr;
  struct pptr_list_s* next;
};

typedef struct mchunk_s mchunk_t;
struct mchunk_s {
  unsigned size;
  
  pptr_list_t* free;
  pptr_list_t* unused;
};

static void pcollect(memory_t* mp) {
  // @todo
  mp->cc_count_down = CC_FREQ;
}

static void pmemcpy(pptr_t* dst, pptr_t* src) {
  unsigned size = src->size < dst->size ? src->size : dst->size;
  memcpy(pptr_cptr(dst), pptr_cptr(src), size);
}

static inline void try_cc(memory_t* mp) {
  if (--(mp->cc_count_down) == 0)
    pcollect(mp);
}

memory_t* pminit() {
  memory_t* mp = malloc(sizeof(memory_t));
  if (mp == NULL)
    return NULL;

  mp->chunks = malloc(sizeof(mchunk_t*) * N_CHUNKS);
  if (mp->chunks == NULL) {
    free(mp);
    return NULL;
  }

  mp->n_chunks = 0;
  mp->n_chunks_limits = N_CHUNKS;
  mp->cc_count_down = CC_FREQ;

  return mp;
}

static pptr_list_t* pptr_list(pptr_t* p, pptr_list_t* next) {
  pptr_list_t* np = malloc(sizeof(pptr_list_t));
  if (np == NULL)
    return NULL;
  
  np->ptr = p;
  np->next = next;
}

#define MIN_ALLOC_SIZE 4096
static mchunk_t* alloc_chunk(unsigned n_bytes) {
  unsigned alloc_size = n_bytes + sizeof(pptr_t);
  if (alloc_size < MIN_ALLOC_SIZE)
    alloc_size = MIN_ALLOC_SIZE;
  
  mchunk_t* cp = malloc(sizeof(mchunk_t) + alloc_size + sizeof(pptr_t));
  if (cp == NULL)
    return NULL;

  byte_t* head = (byte_t*)(cp + 1);
  pptr_t* p = pptr((byte_t*)head, 0, n_bytes);
  cp->free = pptr_list(p, NULL);
  if (cp->free == NULL) {
    free(cp);
    return NULL;
  }
  
  cp->size = n_bytes;
  cp->unused = NULL;

  // boundary sentinel
  pptr(head, alloc_size, 0);

  return cp;
}

pptr_t* pmalloc(memory_t* mp, size_t n_bytes) {
  mchunk_t* cp;
  pptr_t* p;
  pptr_list_t** ip;

  for (unsigned i = 0; i < mp->n_chunks; i++) {
    cp = mp->chunks[i];
    if (cp->size < n_bytes)
      continue;
    ip = &cp->free;
    while ((*ip)->ptr->size < n_bytes)
      ip = &(*ip)->next;
    if (*ip != NULL)
      goto extract_pptr;
  }
  
  cp = alloc_chunk(n_bytes);
  if (cp == NULL)
    return NULL;
  ip = &cp->free;
  mp->chunks[mp->n_chunks++] = cp;
  
  if (mp->n_chunks == mp->n_chunks_limits) {
    mp->n_chunks_limits = mp->n_chunks_limits << 1;
    pcollect(mp);
  }

extract_pptr:
  pptr_t* rp;
  
  p = (*ip)->ptr;
  rp = pptr_cut(p, n_bytes);

  if (rp == NULL) {
    pptr_list_t* tp = *ip;
    *ip = (*ip)->next;
    tp->next = cp->unused;
    cp->unused = tp;
  }
  else {
    (*ip)->ptr = rp;
    cp->size = cp->size - sizeof(pptr_t);
  }

  cp->size = cp->size - p->size;
  p->free = false;
  return p;
}

pptr_t* prealloc(memory_t* mp, pptr_t* p, size_t n_bytes) {
  if (p->size == n_bytes)
    return p;

  mchunk_t* cp = pptr_head(p);
  pptr_t* rp;

  if (p->size < n_bytes) {
    unsigned alloc_size = p->size + rp->size + sizeof(pptr_t);
    rp = pptr_next(p);
    
    if (rp && rp->free && alloc_size >= n_bytes) {
      pptr_list_t** ip = &cp->free;
      pptr_list_t* tp;
      while ((*ip)->ptr != rp)
        ip = &(*ip)->next;
      tp = *ip;
      *ip = tp->next;
      tp->next = cp->unused;
      cp->unused = tp;
      p->size = alloc_size;

      return prealloc(mp, p, n_bytes);
    }

    rp = pmalloc(mp, n_bytes);
    pmemcpy(rp, p);

    return rp;
  }

  rp = pptr_cut(p, n_bytes);
  if (!rp)
    return p;
  pfree(mp, rp);

  return p;
}

void pfree(memory_t* mp, pptr_t* p) {
  mchunk_t* cp = pptr_head(p);
  p->free = true;
  
  if (cp->unused) {
    pptr_list_t* tp = cp->unused;
    tp->ptr = p;
    cp->unused = tp->next;
    tp->next = cp->free;
    cp->free = tp;
  }
  else 
    cp->free = pptr_list(p, cp->free);
  
  try_cc(mp);
}

void pmclean(memory_t* mp) {
  if (mp == NULL)
    return;
  
  free(mp->chunks);
  free(mp);
}