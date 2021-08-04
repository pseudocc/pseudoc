/**
 * @file p_memory.h
 * @brief memory management things for pseudoc
 * 
 * @author Yu Ze <pseudoc@163.com>
 * @date Tue Jul 13 2021 15:18:12 GMT+0800 (China Standard Time)
 **/

#ifndef PSEUDO_MEMORY_H
#define PSEUDO_MEMORY_H

#include <stdlib.h>

#define MIN_ALLOC_SIZE 4096

typedef unsigned char byte_t;

/**
 * internal type that hold the memory block
 **/
typedef struct chunk chunk_t;

/**
 * `chunk_t` base address with an offset
 **/
typedef struct atom atom_t;

struct atom {
  chunk_t* from;
  size_t offset;
  size_t size;
};

/**
 * enum for GC level
 **/
typedef enum gc_level{
  gc_quick = 0b0001,
  gc_regular = 0b0010,
  gc_clean = 0b0100,
  gc_instant = 0b1000
} gc_level_t;

/**
 * internal allocation method to create more chunks
 **/
static chunk_t* chunk_alloc(size_t n_bytes);

/**
 * free a chunk that does not mean call the native
 * `free` it is actually added to the pending list
 **/
static void chunk_free(chunk_t* cp);

/**
 * memoery allocation that try to find a free chunk
 * first, otherwise call native `malloc`
 * 
 * @return EXIT_SUCCESS | OUT_OF_MEMORY
 **/
int pseudo_malloc(size_t n_bytes, atom_t* r_atom);

/**
 * sizeable allocation that does not share this chunk
 * of memory with other variables
 * 
 * @return EXIT_SUCCESS | OUT_OF_MEMORY
 **/
int pseudo_salloc(size_t n_bytes, atom_t* r_atom);

/**
 * re-allocate sizeable allocation that only call native
 * `realloc` when `n_bytes` is greater than before
 * 
 * @return EXIT_SUCCESS | OUT_OF_MEMORY | MAJOR_PROBLEM
 **/
int pseudo_realloc(size_t n_bytes, atom_t* r_atom);

/**
 * mark atom free if the whole chunk is free now
 * add it to the `free_list`
 * 
 * @return EXIT_SUCCESS | OUT_OF_MEMORY |
 * MINOR_PROBLEM | MAJOR_PROBLEM
 **/
int pseudo_mark_free(const atom_t* r_atom);

/**
 * garbage collection
 **/
void pseudo_gcollect(gc_level_t lvl);

#endif
