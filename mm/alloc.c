#include <mm/alloc.h>
#include <stdlib.h>

inline caddr_t std_malloc(uint64_t size) {
  caddr_t p = malloc(size);
  if (p == NULL)
    return null;
  return p;
}
