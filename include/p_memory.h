#ifndef PSEUDOC_MEMORY_H
#define PSEUDOC_MEMORY_H

#include "primitive.h"

typedef struct p_memory p_memory;

p_memory* create_memblock(int max_bytes);

p_int   memget_int(p_memory* m, int pp);
p_byte  memget_byte(p_memory* m, int pp);
p_float memget_float(p_memory* m, int pp);
p_array memget_array(p_memory* m, int pp);

void memset_int(p_memory* m, int pp, p_int val);
void memset_byte(p_memory* m, int pp, p_byte val);
void memset_float(p_memory* m, int pp, p_float val);

int memnew_int(p_memory* m);
int memnew_byte(p_memory* m);
int memnew_float(p_memory* m);
int memnew_array(p_memory* m, int n_bytes);

void memfree_array(p_memory* m, int pp);
void free_memblock(p_memory* m);

#define N_BYTES(t1) sizeof(t1) / sizeof(p_byte)

#endif
