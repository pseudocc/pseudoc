#include <mm/alloc.h>

#define MAX_STACK_DEPTH (1 << 10)
#define MIN_STACK_SIZE (1 << 23)

static caddr_t stack_head = null;
static int32_t stack_ptrs[MAX_STACK_DEPTH];
static int16_t stack_depth = -1;
static caddr_t stack_end;

int32_t stack_init(uint32_t size) {
  if (stack_head != null)
    return PR_NOTHING;
  if (size < MIN_STACK_SIZE)
    size = MIN_STACK_SIZE;
  caddr_t p = std_malloc(size);
  if (p == null)
    return PR_OUTOFMEM;
  stack_head = p;
  stack_ptrs[stack_depth = 0] = 0;
  return PR_SUCCESS;
}

#define STACK_ALIGN_BYTES 4
caddr_t stack_alloc(uint32_t n_bytes) {
  int32_t stack_top;
  uint32_t n_units = n_bytes / STACK_ALIGN_BYTES;
  if (n_bytes % STACK_ALIGN_BYTES != 0)
    n_units++;
  n_bytes = n_units * STACK_ALIGN_BYTES;
  stack_top = stack_ptrs[stack_depth++] + n_bytes;
  stack_ptrs[stack_depth] = stack_top;
  return (uint8_t*)(stack_head) + stack_top;
}

void stack_dealloc() {
  stack_depth--;
}
