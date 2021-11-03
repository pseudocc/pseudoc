#ifndef PSEUDOC_ADDRESS_H
#define PSEUDOC_ADDRESS_H

#include <ptypes.h>

typedef void* caddr_t;
typedef struct paddr_s paddr_t;

#define PAGE_ORDER 12
#define PLV2_ORDER 20
#define PLV1_ORDER 24

struct paddr_s {
  uint16_t offset: PAGE_ORDER;
  uint32_t p2: PLV2_ORDER;
  uint32_t p1: PLV1_ORDER;
  uint8_t prot;
};

enum {
  PROT_NONE = 0,
  PROT_READ = 1,
  PROT_WRITE = 2,
  PROT_EXEC = 4,
};

static const paddr_t nullp = { .prot = 0 };

#endif
