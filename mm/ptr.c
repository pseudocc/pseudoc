#include <mm/ptr.h>
#include <mm/page.h>

static int32_t caddr(paddr_t addr, caddr_t* r) {
  page_t p;
  int32_t pr = page_get(addr.p1, addr.p2, &p);
  if (pr)
    return pr;
  *r = page_head(&p);
  return PR_SUCCESS;
}

int32_t ptr_get_int32(paddr_t addr, int32_t* v) {
  caddr_t p;
  int32_t pr = caddr(addr, &p);
  if (pr)
    return pr;
  *v = *(int32_t*)p;
  return PR_SUCCESS;
}
