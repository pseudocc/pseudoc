#include <mm/page.h>
#include <stdio.h>

FILE* f;

static int32_t page_swap_init(char* fp) {
  f = fopen(fp, "w+");
  if (f == null)
    return PR_FILECREATE;
  return PR_SUCCESS;
}

int32_t page_swap_in(page_t* p, int32_t doff) {
  caddr_t cp = page_head(p);
  if (cp == null)
    return PR_NULLPTR;
  int32_t r = fseek(f, doff << PAGE_ORDER, SEEK_SET);
  if (r)
    return PR_FILESEEK;
  uint64_t bytes_read = fread(cp, PAGE_SIZE, 1, f);
  if (bytes_read != PAGE_SIZE)
    return PR_FILEREAD;
  return PR_SUCCESS;
}

int32_t page_swap_out(page_t* p, int32_t doff) {
  caddr_t cp = page_head(p);
  if (cp == null)
    return PR_NULLPTR;
  int32_t r = fseek(f, doff << PAGE_ORDER, SEEK_SET);
  if (r)
    return PR_FILESEEK;
  uint64_t bytes_wrote = fwrite(cp, PAGE_SIZE, 1, f);
  if (bytes_wrote != PAGE_SIZE)
    return PR_FILEWRITE;
  return PR_SUCCESS;
}
