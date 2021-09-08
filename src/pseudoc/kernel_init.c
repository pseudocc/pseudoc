#include <pseudoc/kernel.h>

#define QMEM_SIZE 512

void pkinit(char** args, int argc) {
  memory = pminit(QMEM_SIZE);
  if (memory == NULL)
    return;
  
  
}
