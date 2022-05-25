#include "common.h"

static uint hash_u32(uint x) { return 0x01008041 * x; }

static uint hash_u64(u64 x) { return hash_u32((uint)x ^ (uint)(x >> 32)); }

#if 0
static uint hash_mem(void* v, size_t l) {
  uint h = 0;
  void* end = v + l;
  for (; v < end; h ^= hash_u32(v))
    ;
}
#endif

static uint hash_pointer(void* x) {
  return ((sizeof(x) <= 4) ? hash_u32((uint)(uintptr_t)x)
                           : hash_u64((u64)(uintptr_t)x));
}

