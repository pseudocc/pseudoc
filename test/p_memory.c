#include <stdio.h>
#include "p_test.h"
#include "p_memory.h"

/**
 * check if these two tiny atom is from
 * the same chunk
 **/
int malloc_tiny2_atoms() {
  int code;
  atom_t** atoms = malloc(sizeof(atom_t*) * 2);
  printf("malloc_tiny2_atoms\n");

  code = pseudo_malloc(1, atoms[0]);
  if (code) {
    printf("Allocate first atom with error: %d\n", code);
    return code;
  }

  code = pseudo_malloc(1, atoms[1]);
  if (code) {
    printf("Allocate first atom with error: %d\n", code);
    return code;
  }
  
  // atoms[0]->from == atoms[1]->from
  if (*(void**)(atoms[0]) != *(void**)(atoms[0])) {
    printf("2 atoms are not from the same chunk, unexpected.\n");
    return -1;
  }
  
  return 0;
}

int p_memory_ut() {
  int (*uts[])() = { &malloc_tiny2_atoms };
  int n_cases = sizeof(uts) / sizeof(uts[0]);
  
  for (int i = 0; i < n_cases; i++) {
    int exit_code = (uts[i])();
    printf("Memory UT %d: exit with %d.", i, exit_code);
    if (exit_code)
      return exit_code;
  }

  return 0;
}