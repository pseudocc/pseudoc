#include <mm/alloc.h>
#include <mm/page.h>
#include <mm/ptr.h>

typedef struct alloc_s alloc_t;

#define SORT_FREQ 128

static alloc_t* free_allocs = null;
static alloc_t* used_allocs = null;

static uint64_t ppos = 0;
static uint64_t* pposp = &ppos;
static uint16_t sort_counter = 0;

#define MIN_ARENA_SIZE (PAGE_SIZE << 8)

struct alloc_s {
  paddr_t pptr;
  uint32_t n_refs;
  uint32_t size;
  alloc_t* next;
  alloc_t* prev;
};

int32_t memory_init(uint64_t size) {
  if (size < MIN_ARENA_SIZE)
    size = MIN_ARENA_SIZE;
  else {
    uint64_t n_pages = size / PAGE_SIZE;
    if (size % PAGE_SIZE)
      n_pages++;
    size = n_pages * PAGE_SIZE;
  }
  return PR_SUCCESS;
}

#define MALLOC_ALIGN_BYTES 4
#define MIN_ALLOC_SIZE (MALLOC_ALIGN_BYTES + sizeof(alloc_t))

paddr_t malloc(uint32_t n_bytes) {
  if (n_bytes == 0)
    return nullp;
  
  uint32_t n_units = n_bytes / MALLOC_ALIGN_BYTES;
  if (n_bytes % MALLOC_ALIGN_BYTES)
    n_units++;
  n_bytes = n_units * MALLOC_ALIGN_BYTES;

  alloc_t* p = free_allocs;
  while (p != null) {
    if (p->size >= n_bytes) {
      alloc_t** pnp = p->prev == null ? &free_allocs : &p->prev->next;
      *pnp = p->next;
      if (p->next != null)
        p->next->prev = p->prev;
        
      uint32_t rest = p->size - n_bytes;
      if (rest >= MIN_ALLOC_SIZE) {
        alloc_t* np = std_malloc(sizeof(alloc_t));
        if (np != null) {
          uint64_t nppos = *(uint64_t*)(&p->pptr) + p->size;
          np->n_refs = 0;
          np->pptr = *(paddr_t*)&nppos;
          np->size = rest;
          np->next = free_allocs;
          np->prev = null;

          free_allocs = np;
          p->size = n_bytes;
        }
      }
      return p->pptr;
    }
    p = p->next;
  }

  p = std_malloc(sizeof(alloc_t));
  if (p == null)
    return nullp;

  paddr_t pp = *(paddr_t*)pposp;
  pp.prot = PROT_READ | PROT_WRITE;

  p->n_refs = 0;
  p->pptr = pp;
  p->size = n_bytes + sizeof(alloc_t*);
  p->next = used_allocs;
  p->prev = null;

  if (used_allocs != null)
    used_allocs->prev = p;
  used_allocs = p;

  ppos += p->size;
  return p->pptr;
}

void free(paddr_t addr) {
  alloc_t* p;
  ptr_get_uint64(addr, (uint64_t*)p);

  if (p == null || p->n_refs < 0)
    return;
  p->n_refs = -1;

  alloc_t** pnp = p->prev == null ? &used_allocs : &p->prev->next;
  *pnp = p->next;
  
  if (p->next != null)
    p->next->prev = p->prev;

  p->prev = null;
  p->next = free_allocs;
  free_allocs = p;

  if (++sort_counter > SORT_FREQ)
    memory_sort();
}

static alloc_t* partition(alloc_t** istart, alloc_t* end) {
  alloc_t* pivot = *istart;
  alloc_t* np = pivot->next;
  alloc_t** iend;

  pivot->next = end;
  iend = &pivot->next;

  while (np != end) {
    alloc_t* nnp = np->next;
    if (np->size < pivot->size) {
      np->next = *istart;
      *istart = np;
    }
    else {
      np->next = *iend;
      *iend = np;
      iend = &np->next;
    }
    np = nnp;
  }

  return pivot;
}

static void alloc_qsort(alloc_t** istart, alloc_t* end) {
  if (*istart == end || (*istart)->next == end)
    return;
  
  alloc_t* pivot = partition(istart, end);
  alloc_qsort(istart, pivot);
  alloc_qsort(&pivot->next, end);
}

static void memory_sort() {
  alloc_t* prevp = null;
  alloc_t* p;
  
  alloc_qsort(&free_allocs, null);
  p = free_allocs;

  while (p != null) {
    p->prev = prevp;
    prevp = p;
    p = p->next;
  }
}
