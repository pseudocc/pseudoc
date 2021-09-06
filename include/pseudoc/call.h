#ifndef PSEUDOC_CALL_H
#define PSEUDOC_CALL_H

#include <pseudoc/types.h>
#include <pseudoc/mm.h>

typedef struct call_s call_t;
typedef struct call_stack_s call_stack_t;
typedef struct manifest_s manifest_t;

struct call_s {
  byte_t flags;
};

call_stack_t* cinit(memory_t* mp, manifest_t* mfp);
call_t* cnext(call_stack_t* sp);

#endif
