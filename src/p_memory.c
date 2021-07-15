#include "p_memory.h"
#include "p_system.h"

static unsigned int n_chunks;
static unsigned int n_chunks_limit;

struct chunk_usage {
  unsigned int size;
  void* end;
  struct chunk_usage* next;
};

struct chunk {
  void* head;
  /**
   * number of bytes for the free memory blocks
   **/
  size_t size;
  /**
   * use head and usage_list->size and usage_list->end
   * to find the free momory block
   * 
   * this field will always be not NULL
   * a safeguard is initialized
   **/
  struct chunk_usage* usage_list;
  /**
   * when an `atom_t` is marked free, append it to this
   * will merge into the `usage_list` lazily when gc is called
   **/
  struct chunk_usage* freed_list;
  struct chunk* next;
  /**
   * number of references by the variables
   **/
  unsigned int n_refs;
};

struct atom {
  chunk_t* from;
  size_t offset;
  size_t size;
};

static chunk_t guardian = { .size = 0 };
static chunk_t* memory_pool = &guardian;

#define MIN_ALLOC_SIZE 1024
#define CHUNK_FULLSIZE (sizeof(chunk_t) + sizeof(struct chunk_usage))

static chunk_t* chunk_alloc(size_t n_bytes) {
  if (n_bytes < MIN_ALLOC_SIZE)
    n_bytes = MIN_ALLOC_SIZE;

  chunk_t* cp = malloc(n_bytes + CHUNK_FULLSIZE);
  if (cp == NULL)
    return NULL;
  
  cp->head = (byte_t*)cp + CHUNK_FULLSIZE;
  cp->size = n_bytes;
  cp->n_refs = 0;
  cp->next = NULL;

  // initialize the usage safeguard
  cp->usage_list = (byte_t*)cp + sizeof(chunk_t);
  cp->usage_list->size = 0;
  cp->usage_list->end = (byte_t*)cp->head + n_bytes;
  cp->usage_list->next = NULL;
  
  chunk_free(cp);
  n_chunks++;

  return cp;
}

static void chunk_free(chunk_t* cp) {
  struct chunk_usage* cup = cp->usage_list;
  struct chunk_usage* cfp = cp->freed_list;
  chunk_t* p = memory_pool;
  cp->n_refs = 0;
  
  if (cp->next == NULL)
    cp->next = p;

  while (cup->next) {
    // releases every usage except the safeguard
    cp->usage_list = cup->next;
    free(cup);
    cup = cp->usage_list;
  }

  while (cfp) {
    cp->freed_list = cfp->next;
    free(cfp);
    cfp = cp->freed_list;
  }

  memory_pool = cp;
}

int pseudo_malloc(size_t n_bytes, atom_t* r_atom) {
  // indirect pointer
  struct chunk_usage** ip;
  chunk_t* cp = memory_pool;
  void* start;

FIND_CHUNK:
  // from existed free blocks
  while (cp) {
    if (cp->size >= n_bytes) {
      ip = &cp->usage_list;
      start = cp->head;
      
      while (*ip) {
        // have enough space
        if ((byte_t*)start + n_bytes + (*ip)->size <= (*ip)->end) 
          goto EXTRACT_ATOM;
        start = (*ip)->end;
        ip = &(*ip)->next;
      }
    }
    cp = cp->next;
  }

  if (n_chunks > n_chunks_limit) {
    pseudo_gcollect(gc_regular | gc_instant);
    n_chunks_limit = n_chunks_limit << 1;
    
    goto FIND_CHUNK;
  }

  cp = chunk_alloc(n_bytes);
  if (cp == NULL)
    return OUT_OF_MEMORY;
  
  ip = &cp->usage_list;
  start = cp->head;

EXTRACT_ATOM:
  // `*ip` is not the safeguard
  if ((*ip)->next) {
    (*ip)->size += n_bytes;
    (*ip)->end = (byte_t*)start + n_bytes;
  }
  else {
    struct chunk_usage* used = malloc(sizeof(struct chunk_usage));
    if (used == NULL)
      return OUT_OF_MEMORY;

    used->size = n_bytes;
    used->end = (byte_t*)start + n_bytes;

    struct chunk_usage* tp = *ip;
    *ip = used;
    used->next = tp;
  }

  *r_atom = (atom_t){
    .from = cp,
    .offset = (byte_t*)start - (byte_t*)cp->head,
    .size = n_bytes
  };
  cp->size -= n_bytes;
  cp->n_refs++;

  return EXIT_SUCCESS;
}

int pseudo_salloc(size_t n_bytes, atom_t* r_atom) {
  chunk_t* cp = chunk_alloc(n_bytes);
  if (cp == NULL)
    return OUT_OF_MEMORY;
  
  struct chunk_usage* used = malloc(sizeof(struct chunk_usage));
  if (used == NULL)
    return OUT_OF_MEMORY;

  used->next = cp->usage_list;
  used->size = n_bytes;
  used->end = (byte_t*)cp->head + n_bytes;
  cp->usage_list = used;

  return EXIT_SUCCESS;
}

int pseudo_realloc(size_t n_bytes, atom_t* r_atom) {
  if (r_atom == NULL || r_atom->from == NULL || r_atom->offset)
    return MAJOR_PROBLEM;

  if (!n_bytes)
    chunk_free(r_atom->from);
  
  chunk_t* cp = realloc(r_atom->from, n_bytes + CHUNK_FULLSIZE);
  if (cp == NULL)
    return OUT_OF_MEMORY;
  
  chunk_free(cp);
  if (cp != r_atom->from) {
    cp->head = (byte_t*)cp + CHUNK_FULLSIZE;
    cp->usage_list = (byte_t*)cp + sizeof(chunk_t);
  }
  
  cp->size = n_bytes;
  cp->usage_list->end = (byte_t*)cp->head + n_bytes;

  return pseudo_malloc(n_bytes, r_atom);
}

int pseudo_mark_free(const atom_t atom) {
  if (atom.from == NULL || !atom.size)
    return MAJOR_PROBLEM;
  
  chunk_t* cp = atom.from;
  void* start = cp->head;
  void* free_start = (byte_t*)start + atom.offset;
  void* free_end = (byte_t*)free_start + atom.size;
  struct chunk_usage** ip = &cp->freed_list;

  while (*ip) {
    if ((byte_t*)free_end + (*ip)->size <= (*ip)->end)
      break;
    start = (*ip)->end;
    ip = &(*ip)->next;

    if (start > free_start)
      // already freed
      return MINOR_PROBLEM;
  }

  if ((*ip) && (byte_t*)free_end + (*ip)->size == (*ip)->end)
    (*ip)->size += atom.size;
  else {
    struct chunk_usage* tup = malloc(sizeof(struct chunk_usage));
    if (tup == NULL)
      return OUT_OF_MEMORY;
    
    tup->end = free_end;
    tup->next = *ip;
    tup->size = atom.size;

    *ip = tup;
  }

  return EXIT_SUCCESS;
}

enum gc_level {
  gc_quick = 0b0001,
  gc_regular = 0b0010,
  gc_clean = 0b0100,
  gc_instant = 0b1000
};

void pseudo_gcollect(gc_level_t lvl) {
  if (lvl & gc_quick) {
    chunk_t** ip = &memory_pool;
    while (*ip) {
      if (!(*ip)->n_refs) {
        chunk_t* cp = *ip;
        *ip = (*ip)->next;
        cp->next = NULL;

        if (lvl & gc_clean)
          free(cp);
        else 
          chunk_free(cp);

        if (lvl & gc_instant)
          return;
      }
      ip = &(*ip)->next;
    }
  }
  if (lvl & gc_regular) {

  }
}
