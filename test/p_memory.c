/**
 * @file p_memory.c
 * @brief memory management UT
 * 
 * @author Yu Ze <pseudoc@163.com>
 * @date Wed Aug 04 2021 21:52:41 GMT+0800 (China Standard Time)
 **/

#include <stdio.h>
#include "p_test.h"
#include "p_memory.h"

/**
 * check if these two tiny atom is from
 * the same chunk
 **/
int malloc_tiny2_atoms() {
  int code;

  atom_t* atoms = malloc(sizeof(atom_t) * 2);
  printf("malloc_tiny2_atoms\n");

  code = pseudo_malloc(1, &atoms[0]);
  if (code) {
    printf("Allocate the first atom with error: %d\n", code);
    return code;
  }

  code = pseudo_malloc(1, &atoms[1]);
  if (code) {
    printf("Allocate the second atom with error: %d\n", code);
    return code;
  }
  
  if (atoms[0].from != atoms[1].from) {
    printf("2 atoms are not from the same chunk, unexpected.\n");
    return -1;
  }

  for (int i = 0; i < 2; i++) {
    code = pseudo_mark_free(&atoms[0]);
    if (code) {
      printf("Mark free for atom %d with error: %d\n", i, code);
      return code;
    }
  }

  pseudo_gcollect(gc_regular | gc_clean);
  free(atoms);
  
  return 0;
}

/**
 * salloc->realloc is the safe way
 * malloc->realloc might throw an error
 **/
int realloc_full_test() {
  int code;
  atom_t m_atom, s_atom;

  printf("realloc_atom\n");

  code = pseudo_salloc(1, &s_atom);
  if (code) {
    printf("Allocate s_atom with error: %d\n", code);
    return code;
  }

  code = pseudo_malloc(1, &m_atom);
  if (code) {
    printf("Allocate m_atom with error: %d\n", code);
    return code;
  }

  code = pseudo_realloc(10, &m_atom);
  if (!code) {
    printf("Re-allocate should throw an error, unexpected.\n");
    return -1;
  }

  code = pseudo_realloc(10, &s_atom);
  if (code) {
    printf("Re-allocate s_atom with error: %d\n", code);
    return -1;
  }
  
  // this part is already tested in malloc_tiny2_atoms
  pseudo_mark_free(&m_atom);
  pseudo_mark_free(&s_atom);
  pseudo_gcollect(gc_regular | gc_clean);

  return 0;
}

/**
 * regular_check_free contains too many
 * complicated logics
 * we use this to make sure it works well
 **/
int gc_regular_test() {

}

int p_memory_ut() {
  int (*uts[])() = { 
    &malloc_tiny2_atoms,
    &realloc_full_test
  };
  int n_cases = sizeof(uts) / sizeof(uts[0]);
  
  for (int i = 0; i < n_cases; i++) {
    int exit_code = (uts[i])();
    printf("Memory UT %d: exit with %d.\n", i, exit_code);
    if (exit_code)
      return exit_code;
  }

  return 0;
}
