#ifndef PSEUDOC_SCOPE_H
#define PSEUDOC_SCOPE_H

#include "p_memory.h"
#include "decl.h"

typedef struct p_scope p_scope;

p_scope* create_scope(p_scope* parent);
void scope_declare(p_scope* s, p_decl* d);
void free_scope(p_scope* s);

#endif
