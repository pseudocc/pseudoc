#ifndef PSEUDOC_PAGE_H
#define PSEUDOC_PAGE_H

#include <mm/addr.h>

typedef struct page_s page_t;

struct page_s {
  uint64_t frame: 52;
  uint16_t n_refs: 10;
  uint8_t lock: 1;
  uint8_t off_sync: 1;
};

#define PAGE_SIZE (1 << PAGE_ORDER)

int32_t page_init(uint8_t order);
caddr_t page_head(page_t* p);

int32_t page_get(int32_t p1, int32_t p2, page_t** p);
int32_t page_set(int32_t p1, int32_t p2, page_t* p);

int32_t page_swap_in(page_t* p, int32_t fd, int32_t offset);
int32_t page_swap_out(page_t* p, int32_t fd, int32_t offset);

#endif
