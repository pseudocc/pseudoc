#ifndef PSEUDOC_ADDRESS_H
#define PSEUDOC_ADDRESS_H

#include <ptypes.h>

typedef void* caddr_t;
typedef struct paddr_s paddr_t;

struct paddr_s {
  uint16_t offset: 12;
  uint32_t p2: 20;
  uint32_t p1: 24;
  uint8_t prot;
};

enum {
  PROT_READ = 1,
  PROT_WRITE = 2,
  PROT_EXEC = 4,
};

static const paddr_t nullp = { .prot = 0xff };
inline int32_t paddr_is_null(paddr_t addr) {
  return addr.prot == 0xff;
}

#endif
