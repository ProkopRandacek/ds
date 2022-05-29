#include <stdlib.h>

static void* xmalloc(size_t s) {
  void* d = malloc(s);
  if (!d)
    __builtin_trap();
  return d;
}

static void xfree(void* ptr) { free(ptr); }

static void* xrealloc(void* p, size_t s) {
  void* d = realloc(p, s);
  if (!d)
    __builtin_trap();
  return d;
}
