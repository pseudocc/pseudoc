#include <mm/page.h>
#include <stdlib.h>

static uint8_t* fhead;

#define INITIAL_PTBL_SIZE 128
#define MIN_INIT_ORDER 12

static page_t** ptbl;
static uint32_t ptbl_size;

static inline page_t* more_page() {
  return calloc(1 << PLV2_ORDER, sizeof(page_t));
}

int32_t page_init(uint8_t order, char* fp) {
  uint64_t n_pages = 1 << (order < MIN_INIT_ORDER ? MIN_INIT_ORDER : order);
  uint64_t n_bytes = n_pages << PAGE_ORDER;

  fhead = aligned_alloc(PAGE_SIZE, n_bytes);
  if (fhead == null)
    return PR_OUTOFMEM;

  ptbl_size = INITIAL_PTBL_SIZE;
  ptbl = calloc(ptbl_size, sizeof(page_t*));

#define PLV2_MASK ((1 << PLV2_ORDER) - 1)
#define PLV1_MASK ((1 << PLV1_ORDER) - 1)
  
  for (uint64_t i = 1; i < n_pages; i++) {
    int32_t p1 = (i >> PLV2_ORDER) & PLV1_MASK;
    int32_t p2 = i & PLV2_MASK;

    if (ptbl[p1] == null) {
      ptbl[p1] = more_page();
      if (ptbl[p1] == null)
        return PR_OUTOFMEM;
    }

    page_t* p = &ptbl[p1][p2];
    p->header.uint64 = (uint64_t)(fhead + (i << PAGE_ORDER));
  }

  int32_t pr = page_swap_init(fp);
  if (pr != PR_SUCCESS)
    return pr;

  return PR_SUCCESS;
}

caddr_t page_head(page_t* p) {
  if (p == null)
    return null;
  if (p->header.value.n_refs < 0x3ff)
    p->header.value.n_refs++;
  return p->header.value.frame << PAGE_ORDER;
}

int32_t page_get(int32_t p1, int32_t p2, page_t* p) {
  if (ptbl[p1] == null) {
    ptbl[p1] = more_page();
    if (ptbl[p1] == null)
      return PR_OUTOFMEM;
  }
  page_t* pp = &ptbl[p1][p2];
  if (pp->header.value.frame == 0) {
    int32_t pr = page_swap_in(pp);
    if (pr != PR_SUCCESS)
      return pr;
  }

  *p = *pp;
  return PR_SUCCESS;
}
