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

#define IMPL_PTR_BODY(ptype, assignment) {\
  ptype##_t* p;\
  int32_t pr = caddr(addr, &p);\
  if (pr)\
    return pr;\
  assignment;\
  return PR_SUCCESS;\
}

#define IMPL_PTR_GET(ptype)\
  int32_t ptr_get_##ptype(paddr_t addr, ptype##_t* v) IMPL_PTR_BODY(ptype, *v = *p)
#define IMPL_PTR_SET(ptype)\
  int32_t ptr_set_##ptype(paddr_t addr, ptype##_t v) IMPL_PTR_BODY(ptype, *p = v)

#define IMPL_PTR_OP(ptype)\
  IMPL_PTR_GET(ptype)\
  IMPL_PTR_SET(ptype)

#define IMPL_INT_PTR_OP(prefix)\
  IMPL_PTR_OP(prefix##8)\
  IMPL_PTR_OP(prefix##16)\
  IMPL_PTR_OP(prefix##32)\
  IMPL_PTR_OP(prefix##64)

IMPL_PTR_OP(float32)
IMPL_PTR_OP(float64)

IMPL_INT_PTR_OP(int)
IMPL_INT_PTR_OP(uint)
