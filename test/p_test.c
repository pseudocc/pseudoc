#include <stdio.h>
#include "p_test.h"

int main() {
  int (*uts[])() = { &p_memory_ut };
  int n_cases = sizeof(uts) / sizeof(uts[0]);
  
  for (int i = 0; i < n_cases; i++) {
    int exit_code = (uts[i])();
    printf("UT %d: exit with %d.", i, exit_code);
    if (exit_code)
      return exit_code;
  }

  return 0;
}
