#include <mm/page.h>
#include <stdio.h>

static FILE* f;
static uint32_t dpos;

#define SWAP_HEAP_SIZE 16
static page_t* swap_heap[SWAP_HEAP_SIZE];
static int32_t heap_size;

static inline int32_t page_cmp(page_t lhs, page_t rhs) {
  return lhs.header.value.n_refs - rhs.header.value.n_refs;
}

static inline void build_heap() {
  for (int32_t i = heap_size / 2 - 1; i >= 0; i--)
    heapify(heap_size, i);
}

static inline void sort_heap() {
  page_t* p;

  for (int32_t i = heap_size - 1; i > 0; i--) {
    p = swap_heap[0];
    swap_heap[0] = swap_heap[i];
    swap_heap[i] = p;
    
    p->header.value.n_refs = 0;
    heapify(i, 0);
  }
}

static page_t* pop_heap() {
  page_t* p;

  if (heap_size == 0)
    page_sweep();

  p = swap_heap[0];
  swap_heap[0] = swap_heap[heap_size - 1];
  swap_heap[heap_size - 1] = p;
  heapify(--heap_size, 0);

  if (p->header.value.n_refs != 0)
    return pop_heap();

  return p;
}

static void heapify(int32_t n, int32_t i) {
  int32_t ii = i;
  int32_t l = (i << 1) + 1;
  int32_t r = l + 1;

  if (l < n && page_cmp(*swap_heap[l], *swap_heap[ii]) > 0)
    ii = l;
  if (r < n && page_cmp(*swap_heap[r], *swap_heap[ii]) > 0)
    ii = r;
  
  if (i != ii) {
    page_t* p = swap_heap[i];
    swap_heap[i] = swap_heap[ii];
    swap_heap[ii] = p;

    heapify(n, ii);
  }
}

void page_sweep() {
  register page_t* p;

  for (uint32_t p1; p1 < ptbl_size; p1++) {
    if (ptbl[p1] == null)
      continue;
    for (uint32_t p2; p2 < (1 << PLV2_ORDER); p2++) {
      p = &ptbl[p1][p2];
      if (p->header.value.frame == 0)
        continue;
      if (heap_size < SWAP_HEAP_SIZE) {
        swap_heap[heap_size++] = p;
        if (heap_size == SWAP_HEAP_SIZE)
          build_heap();
      }
      else if (page_cmp(*p, **swap_heap) < 0) {
        *swap_heap = p;
        heapify(SWAP_HEAP_SIZE, 0);
      }
      else
        p->header.value.n_refs = 0;
    }
  }

  sort_heap();
}

static int32_t page_swap_init(char* fp) {
  f = fopen(fp, "w+");
  if (f == null)
    return PR_FILECREATE;
  dpos = 1;
  heap_size = 0;
  return PR_SUCCESS;
}

static int32_t page_swap_in(page_t* p) {
  if (p == null)
    return PR_NULLPTR;
  int32_t r;
  page_t* sp = pop_heap();
  r = page_swap_out(sp);
  if (r != PR_SUCCESS)
    return r;

  union _page_header t = sp->header;
  sp->header = p->header;
  p->header = t;

  if (p->doff == 0)
    return PR_SUCCESS;
  
  caddr_t cp = page_head(p);
  r = fseek(f, p->doff << PAGE_ORDER, SEEK_SET);
  if (r)
    return PR_FILESEEK;
  uint64_t bytes_read = fread(cp, PAGE_SIZE, 1, f);
  if (bytes_read != PAGE_SIZE)
    return PR_FILEREAD;
  return PR_SUCCESS;
}

static int32_t page_swap_out(page_t* p) {
  caddr_t cp = page_head(p);
  if (cp == null)
    return PR_NULLPTR;
  if (p->doff != 0 && p->header.value.off_sync == 0)
    return PR_SUCCESS;

  p->doff = p->doff == 0 ? dpos++ : p->doff;
  int32_t r = fseek(f, p->doff << PAGE_ORDER, SEEK_SET);
  if (r)
    return PR_FILESEEK;
  uint64_t bytes_wrote = fwrite(cp, PAGE_SIZE, 1, f);
  if (bytes_wrote != PAGE_SIZE)
    return PR_FILEWRITE;
  return PR_SUCCESS;
}
