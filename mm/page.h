#ifndef PSEUDOC_PAGE_H
#define PSEUDOC_PAGE_H

#include <mm/addr.h>

typedef struct page_s page_t;
typedef struct phdr_s phdr_t;

struct phdr_s {
  uint64_t frame: 52;
  uint16_t n_refs: 10;
  uint8_t lock: 1;
  uint8_t off_sync: 1;
};

struct page_s {
  union _page_header {
    phdr_t value;
    uint64_t uint64;
  } header;
  int32_t doff;
};

#define PAGE_SIZE (1 << PAGE_ORDER)

int32_t page_init(uint8_t order, char* fp);
caddr_t page_head(page_t* p);

int32_t page_get(int32_t p1, int32_t p2, page_t* p);
int32_t page_set(int32_t p1, int32_t p2, page_t p);

static int32_t page_swap_init(char* fp);
int32_t page_swap_in(page_t* p, int32_t doff);
int32_t page_swap_out(page_t* p, int32_t doff);

#endif
