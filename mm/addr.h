#ifndef PSEUDOC_ADDRESS_H
#define PSEUDOC_ADDRESS_H

#include <ptypes.h>

typedef void* caddr_t;
typedef struct paddr_s paddr_t;

struct paddr_s {
  uint8_t prot;
  uint32_t p1: 24;
  uint32_t p2: 20;
  uint16_t offset: 12;
};

#endif
