#include <mm/page.h>
#include <stdlib.h>

static uint8_t* fhead;
static page_t* phead;

#define INITIAL_PTBL_SIZE 128

static page_t*** ptbl;
static uint32_t ptbl_size;

static inline page_t** more_page() {
  return calloc(1 << PLV2_ORDER, sizeof(page_t*));
}

int32_t page_init(uint8_t order) {
  uint64_t n_pages = 1 << order;
  uint64_t n_bytes = n_pages << PAGE_ORDER;
  
  fhead = aligned_alloc(PAGE_SIZE, n_bytes << 1);
  if (fhead == null)
    return PR_OUTOFMEM;

  phead = (page_t*)(fhead + n_bytes);
  ptbl_size = INITIAL_PTBL_SIZE;
  ptbl = calloc(ptbl_size, sizeof(page_t**));

#define PLV2_MASK ((1 << PLV2_ORDER) - 1)
#define PLV1_MASK ((1 << PLV1_ORDER) - 1)

  for (uint64_t i = 0; i < n_pages; i++) {
    int32_t p1 = (i >> PLV2_ORDER) & PLV1_MASK;
    int32_t p2 = i & PLV2_MASK;

    if (ptbl[p1] == null) {
      ptbl[p1] = more_page();
      if (ptbl[p1] == null)
        return PR_OUTOFMEM;
    }

    uint64_t* up = (uint64_t*)(phead + i);
    *up = (uint64_t)(fhead + (i << PAGE_ORDER));
    ptbl[p1][p2] = (page_t*)up;
  }

  return PR_SUCCESS;
}

caddr_t page_head(page_t* p) {
  if (p == null)
    return null;
  if (p->n_refs < 0x3ff)
    p->n_refs++;
  return p->frame << PAGE_ORDER;
}

int32_t page_get(int32_t p1, int32_t p2, page_t** p) {
  if (ptbl[p1] == null) {
    ptbl[p1] = more_page();
    if (ptbl[p1] == null)
      return PR_OUTOFMEM;
  }
  *p = ptbl[p1][p2];
  return PR_SUCCESS;
}

int32_t page_set(int32_t p1, int32_t p2, page_t* p) {
  if (ptbl[p1] == null) {
    ptbl[p1] = more_page();
    if (ptbl[p1] == null)
      return PR_OUTOFMEM;
  }
  ptbl[p1][p2] = p;
  return PR_SUCCESS;
}
