#ifndef PSEUDOC_PAGE_H
#define PSEUDOC_PAGE_H

#include <mm/addr.h>

#define PAGE_SIZE 4096

typedef struct page_s page_t;

static caddr_t page_head(page_t* p);

void page_init(int32_t total, int32_t virtual);

page_t* page_get(int32_t p1, int32_t p2);
void page_set(int32_t p1, int32_t p2, page_t* p);

int32_t page_swap_in(page_t* p, int32_t fd, int32_t offset);
int32_t page_swap_out(page_t* p, int32_t fd, int32_t offset);

#endif
