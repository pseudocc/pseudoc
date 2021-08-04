/**
 * @file p_test.c
 * @brief All UT cases for the project
 * 
 * @author Yu Ze <pseudoc@163.com>
 * @date Wed Aug 04 2021 21:52:41 GMT+0800 (China Standard Time)
 **/

#include <stdio.h>
#include "p_test.h"

int main() {
  int (*uts[])() = {
#ifdef PSEUDOC_MEMORY_UT
    &p_memory_ut
#endif
  };

  int n_cases = sizeof(uts) / sizeof(uts[0]);
  
  for (int i = 0; i < n_cases; i++) {
    int exit_code = (uts[i])();
    printf("UT %d: exit with %d.\n", i, exit_code);
    if (exit_code)
      return exit_code;
  }

  return 0;
}
