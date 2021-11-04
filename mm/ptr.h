#ifndef PSEUDOC_POINTER_H
#define PSEUDOC_POINTER_H

#include <mm/page.h>
#include <mm/addr.h>

static int32_t caddr(paddr_t addr, caddr_t* r);

#define DECL_PTR_OP(ptype)\
  int32_t ptr_set_##ptype(paddr_t addr, ptype##_t v);\
  int32_t ptr_get_##ptype(paddr_t addr, ptype##_t* v);

#define DECL_INT_PTR_OP(prefix)\
  DECL_PTR_OP(prefix##8)\
  DECL_PTR_OP(prefix##16)\
  DECL_PTR_OP(prefix##32)\
  DECL_PTR_OP(prefix##64)

DECL_PTR_OP(float32)
DECL_PTR_OP(float64)

DECL_INT_PTR_OP(int)
DECL_INT_PTR_OP(uint)

#undef DECL_PTR_OP
#undef DECL_INT_PTR_OP

#endif
