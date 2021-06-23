#ifndef PSEUDOC_DECL_H
#define PSEUDOC_DECL_H

#include "primitive.h"

typedef struct p_decl p_decl;

p_decl* create_decl(int nhash);
void free_decl(p_decl* d);

#endif
