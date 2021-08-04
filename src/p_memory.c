#include "p_memory.h"
#include "p_system.h"

static unsigned int n_chunks = 0;
static unsigned int n_chunks_limit = 16;

typedef struct chunk_usage usage_t;

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
   * a safeguard is initializedbu
   **/
  usage_t* usage_list;
  /**
   * when an `atom_t` is marked free, append it to this
   * will merge into the `usage_list` lazily when gc is called
   **/
  usage_t* freed_list;
  struct chunk* next;
  /**
   * number of references by the variables
   **/
  unsigned int n_refs;
};

static chunk_t guardian = { .size = 0 };
static chunk_t* memory_pool = &guardian;

#define MIN_ALLOC_SIZE 4096
#define CHUNK_FULLSIZE (sizeof(chunk_t) + sizeof(usage_t))
#define N_BYTES_BETWEEN(vp1, vp2) ((byte_t*)vp1 - (byte_t*)vp2)

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
  cp->usage_list = (usage_t*)((byte_t*)cp + sizeof(chunk_t));
  cp->usage_list->size = 0;
  cp->usage_list->end = (byte_t*)cp->head + n_bytes;
  cp->usage_list->next = NULL;
  
  chunk_free(cp);
  n_chunks++;

  return cp;
}

static void chunk_free(chunk_t* cp) {
  usage_t* cup = cp->usage_list;
  usage_t* cfp = cp->freed_list;
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
  cp->size = N_BYTES_BETWEEN(cup->end, cp->head);

  memory_pool = cp;
}

int pseudo_malloc(size_t n_bytes, atom_t* r_atom) {
  // indirect pointer for insertion
  usage_t** ip;
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
        if ((byte_t*)start + n_bytes + (*ip)->size <= (byte_t*)(*ip)->end) 
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
    usage_t* used = malloc(sizeof(usage_t));
    if (used == NULL)
      return OUT_OF_MEMORY;

    used->size = n_bytes;
    used->end = (byte_t*)start + n_bytes;

    usage_t* tp = *ip;
    *ip = used;
    used->next = tp;
  }

  r_atom->from = cp;
  r_atom->offset = N_BYTES_BETWEEN(start, cp->head);
  r_atom->size = n_bytes;

  cp->size -= n_bytes;
  cp->n_refs++;

  return EXIT_SUCCESS;
}

int pseudo_salloc(size_t n_bytes, atom_t* r_atom) {
  if (r_atom == NULL)
    return MAJOR_PROBLEM;
  
  chunk_t* cp = chunk_alloc(n_bytes);
  if (cp == NULL)
    return OUT_OF_MEMORY;
  
  usage_t* used = malloc(sizeof(usage_t));
  if (used == NULL)
    return OUT_OF_MEMORY;

  used->next = cp->usage_list;
  used->size = n_bytes;
  used->end = (byte_t*)cp->head + n_bytes;
  cp->usage_list = used;

  r_atom->from = cp;
  r_atom->offset = 0;
  r_atom->size = n_bytes;

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
    cp->usage_list = (usage_t*)((byte_t*)cp + sizeof(chunk_t));
  }
  
  cp->size = n_bytes;
  cp->usage_list->end = (byte_t*)cp->head + n_bytes;

  return pseudo_malloc(n_bytes, r_atom);
}

int pseudo_mark_free(const atom_t* r_atom) {
  if (r_atom == NULL || r_atom->from == NULL || !r_atom->size)
    return MAJOR_PROBLEM; 
  
  chunk_t* cp = r_atom->from;
  void* start = cp->head;
  void* free_start = (byte_t*)start + r_atom->offset;
  void* free_end = (byte_t*)free_start + r_atom->size;
  // indirect pointer for insertion or update
  usage_t** ip = &cp->freed_list;

  while (*ip) {
    if ((byte_t*)free_end + (*ip)->size <= (byte_t*)(*ip)->end)
      break;
    start = (*ip)->end;
    ip = &(*ip)->next;

    if (start > free_start)
      // already freed
      return MINOR_PROBLEM;
  }

  if ((*ip) && (byte_t*)free_end + (*ip)->size == (*ip)->end)
    (*ip)->size += r_atom->size;
  else {
    usage_t* tup = malloc(sizeof(usage_t));
    if (tup == NULL)
      return OUT_OF_MEMORY;
    
    tup->end = free_end;
    tup->next = *ip;
    tup->size = r_atom->size;

    *ip = tup;
  }

  cp->n_refs--;
  return EXIT_SUCCESS;
}

static int free_memory(int (*predicate)(chunk_t*), gc_level_t lvl) {
  // indirect pointer for removal
  chunk_t** ip = &memory_pool;
  int free_count = 0;
  while (*ip) {
    if (predicate(*ip)) {
      chunk_t* cp = *ip;
      *ip = (*ip)->next;
      cp->next = NULL;

      if (lvl & gc_clean) {
        free(cp);
        n_chunks--;
      }
      else 
        chunk_free(cp);
      free_count++;

      if (lvl & gc_instant)
        return 1;
    }
    ip = &(*ip)->next;
  }

  return free_count;
}

static int quick_check_free(chunk_t* cp) {
  if (!cp->n_refs)
    return 1;
  usage_t* cup = cp->usage_list;
  while (cup->next)
    cup = cup->next;
  return cup->end == (byte_t*)cp->head + cp->size;
}

static int regular_check_free(chunk_t* cp) {
  if (!cp->n_refs)
    return 1;
  usage_t** iup = &cp->usage_list;
  usage_t* cfp = cp->freed_list;

  while ((*iup)->next && cfp) {
    usage_t* tup;
    if ((*iup)->end >= cfp->end) {
      void* uhp = (byte_t*)(*iup)->end - (*iup)->size;
      void* fhp = (byte_t*)cfp->end - cfp->size;
      if (uhp == fhp) {
        if ((*iup)->end == cfp->end) {
          tup = *iup;
          iup = &tup->next;
          free(tup);
        }
        else
          (*iup)->size += cfp->size;
      }
      else if ((*iup)->end != cfp->end) {
        tup = malloc(sizeof(usage_t));
        if (tup == NULL)
          return OUT_OF_MEMORY;
        (*iup)->size = N_BYTES_BETWEEN((*iup)->end, cfp->end);;

        tup->end = fhp;
        tup->size = N_BYTES_BETWEEN(fhp, uhp);
        tup->next = *iup;

        // insert this
        *iup = tup;
        iup = &tup->next;
      }
      else
        (*iup)->end = fhp;
      
      tup = cfp;
      cfp = tup->next;
      free(tup);
      continue;
    }
    
    iup = &(*iup)->next;
  }

  return 0;
}

void pseudo_gcollect(gc_level_t lvl) {
  if (lvl & gc_quick) {
    int free_count = free_memory(&quick_check_free, lvl);
    if (lvl & gc_instant && free_count)
      return;
  }
  if (lvl & gc_regular)
    free_memory(&regular_check_free, lvl);
}
