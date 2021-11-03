#include <mm/alloc.h>
#include <stdlib.h>

inline caddr_t std_malloc(uint64_t size) {
  return malloc(size);
}
