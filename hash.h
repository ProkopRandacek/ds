#ifndef DS_HASH_H
#define DS_HASH_H

#include "common.h"

// taken from https://www.ucw.cz/libucw/
static uint ds_hash_u32(uint x) { return 0x01008041 * x; }
static uint ds_hash_u64(u64 x) { return ds_hash_u32((uint)x ^ (uint)(x >> 32)); }
static uint ds_hash_pointer(void* x) {
  return ((sizeof(x) <= 4) ? ds_hash_u32((uint)(uintptr_t)x)
                           : ds_hash_u64((u64)(uintptr_t)x));
}

// http://www.cse.yorku.ca/~oz/hash.html - djb2
static uint ds_hash_str(char* str) {
  uint hash = 5381;
  int c;

  while ((c = *str++))
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

  return hash;
}

// http://www.cse.yorku.ca/~oz/hash.html - djb2 modified
static uint ds_hash_mem(size_t len, char* str) {
  uint hash = 5381;

  for (size_t i = 0; i < len; i++)
    hash = ((hash << 5) + hash) + str[i]; /* hash * 33 + c */

  return hash;
}

#endif
