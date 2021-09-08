#ifndef PSEUDOC_BUILTIN_H
#define PSEUDOC_BUILTIN_H

#include <pseudoc/types.h>

typedef struct builtin_s builtin_t;
typedef void (*bmethod_t)(void* data, void* rtn);

builtin_t* builtin(bmethod_t** methods, unsigned length);
void pbuiltin(builtin_t* bp, unsigned op, void* data, void* rtn);

#endif
