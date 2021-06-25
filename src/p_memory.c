#include <stdlib.h>
#include <stdio.h>
#include "p_memory.h"

typedef struct {
  p_byte* buffer;
  int position;
  int capacity;
} linear;

void alloc_error() {
  exit(-1);
}

linear* create_linear(int capacity) {
  linear* l = (linear*)malloc(sizeof(linear));
  if (l == NULL) 
    return NULL;
  l->position = 0;
  l->capacity = capacity;
  l->buffer = (p_byte*)malloc(sizeof(p_byte) * l->capacity);
  return l;
}

void lextend(linear* l) {
  l->capacity = l->capacity << 1;
  l->buffer = (p_byte*)realloc(l->buffer, sizeof(p_byte) * l->capacity);
}

int lpush(linear* l, int n_bytes) {
  int pp = l->position;
  l->position += n_bytes;
  if (l->position > l->capacity)
    lextend(l);
  return pp;
}

struct p_memory {
  linear* linear;
  linear* arrays;
};

p_memory* create_memblock(int max_bytes) {
  if (max_bytes < 16)
    max_bytes = 16;
  p_memory* m = (p_memory*)malloc(sizeof(p_memory));
  m->arrays = create_linear(16);
  m->linear = create_linear(max_bytes);
  return m;
}

static inline p_byte* memget_ptr(p_memory* m, int pp) {
  return m->linear->buffer + pp;
}

p_int memget_int(p_memory* m, int pp) {
  return *(p_int*)memget_ptr(m, pp);
}

p_float memget_float(p_memory* m, int pp) {
  return *(p_float*)memget_ptr(m, pp);
}

p_byte memget_byte(p_memory* m, int pp) {
  return *memget_ptr(m, pp);
}

p_array memget_array(p_memory* m, int pp) {
  return *(p_array*)(m->arrays->buffer + pp);
}

void memset_int(p_memory* m, int pp, p_int val) {
  *(p_int*)memget_ptr(m, pp) = val;
}

void memset_float(p_memory* m, int pp, p_float val) {
  *(p_float*)memget_ptr(m, pp) = val;
}

void memset_byte(p_memory* m, int pp, p_byte val) {
  *memget_ptr(m, pp) = val;
}

int memnew_int(p_memory* m) {
  return lpush(m->linear, N_BYTES(p_int));
}

int memnew_byte(p_memory* m) {
  return lpush(m->linear, N_BYTES(p_byte));
}

int memnew_float(p_memory* m) {
  return lpush(m->linear, N_BYTES(p_float));
}

int memnew_array(p_memory* m, int n_bytes) {
  int pp = lpush(m->arrays, N_BYTES(p_array*));
  p_array** ptr = (p_array**)(m->arrays->buffer + pp);
  *ptr = (p_array*)malloc(n_bytes);
  return pp;
}

void memfree_array(p_memory* m, int pp) {
  p_array** ptr = (p_array**)(m->arrays->buffer + pp);
  free(*ptr);
}

void free_memblock(p_memory* m) {
  free(m->linear->buffer);
  free(m->linear);
  m->linear = NULL;
  for (int i = 0; i < m->arrays->position; i += N_BYTES(p_array*))
    if (m->arrays->buffer[i])
      memfree_array(m, i);
  free(m->arrays->buffer);
  free(m->arrays);
  m->arrays = NULL;
}
