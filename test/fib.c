#include "memory.h"
#include <stdio.h>
#include <stdlib.h>

void ctrl_fib(int n) {
  int t;
  int ab[2] = {1, 1};
  while (n--) {
    t = ab[0] + ab[1];
    ab[0] = ab[1];
    ab[1] = t;
  }
  printf("ctrl: %d\n", ab[1]);
}

void expr_fib(int n) {
  p_memory* m = create_memblock(-1);
  int ab_pp = memnew_array(m, N_BYTES(p_int) * 2);
  ((p_int*)memget_array(m, ab_pp))[0] = 1;
  ((p_int*)memget_array(m, ab_pp))[1] = 1;
  int t_pp = memnew_int(m);
  int n_pp = memnew_int(m);
  memset_int(m, n_pp, n);
  while (memget_int(m, n_pp)) {
    memset_int(m, n_pp, memget_int(m, n_pp) - 1);
    memset_int(m, t_pp, ((p_int*)memget_array(m, ab_pp))[0] + ((p_int*)memget_array(m, ab_pp))[1]);
    ((p_int*)memget_array(m, ab_pp))[0] = ((p_int*)memget_array(m, ab_pp))[1];
    ((p_int*)memget_array(m, ab_pp))[1] = memget_int(m, t_pp);
  }
  printf("expr: %d\n", ((p_int*)memget_array(m, ab_pp))[1]);
  free_memblock(m);
}

int main(void) {
  for (int i = 0; i < 5; i++) {
    ctrl_fib(i);
    expr_fib(i);
  }
  return 0;
}
