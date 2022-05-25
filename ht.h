#include "common.h"
#include "hash.h"

#include <assert.h>

/*
 * # Hashtable generator header
 *
 * ## Usage
 *
 * Keeping the keys small is a smart move. Avoid needing to dereference pointer
 * in your custom hash and equal functions.
 *
 * ## Internal workings
 *
 * Hash table with open adressing and linear probing. Keys and values are kept
 * in two separate arrays to keep the linear probing most likely away from RAM.
 * Value lookup will probably be a cache-miss but only one per lookup.
 *
 * ## Required macros
 *
 * HT_PREFIX - value of this is used as the prefix for all functions
 *
 * HT_KEY - the key struct / type
 * HT_VAL - the value struct / type
 *
 * ### Macros that define how to compare and move keys
 *
 * Macro         | Compare | Move    | Note
 * --------------|---------|---------|-------
 * HT_KEY_ATOMIC | `==`    | `=`     | `int`, `char`, `long`, ...
 * HT_KEY_MEM    | memcmp  | memcpy  | HT_KEY_LEN required. (key is char*)
 * HT_KEY_STR    | strncmp | strncpy | HT_KEY_LEN required. (key is char[N])
 * HT_KEY_STRPTR | strcmp  | strcpy  | string outside the key (var len)
 * HT_KEY_CUSTOM | HT_EQ   | HT_CPY  | HT_KEY_DECL required
 *
 * ### Switches
 *
 * HT_BYVAL - Return values in the hash table by value instead of by pointer
 *
 *
 * ### Macros that define what functions you give
 *
 * ### Functions
 *
 * lookup
 * insert
 * update
 * delete
 */

#ifndef HT_KEY
#  error You have to define HT_KEY
#endif

#ifndef HT_VAL
#  error You have to define HT_VAL
#endif

#ifndef HT_KEY_EMPTY
#  error You have to define special grave key value
#endif

#ifndef HT_KEY_GRAVE
#  error You have to define special grave key value
#endif

#ifndef HT_MAX_DENSITY
// Maximum elements/capacity ratio
#  define HT_MAX_DENSITY 0.5
#endif

#ifndef HT_MAX_GRAVE
// Maximum ratio graves/capacity ratio
#  define HT_MAX_GRAVE 0.25
#endif

#define P(x) GLUE_EXPANDED_(HT_PREFIX, x)

// shortcuts
#define K HT_KEY
#define V HT_VAL
#define IS_GRAVE(x) P(eq)(HT_KEY_GRAVE, x)
#define IS_EMPTY(x) P(eq)(HT_KEY_EMPTY, x)

#define MAKE_GRAVE(x) x = HT_KEY_GRAVE
#define MAKE_EMPTY(x) x = HT_KEY_EMPTY

struct P(table) {
  size_t len;    // number of elements in table
  size_t cap;    // capacity of the key&value containers
  size_t graves; // number of graves in the table
  V* vals;
  K* keys;
};

struct P(remove_result) {
  bool removed;
  V v;
};

#define T struct P(table)

#ifndef HT_KEY_CUSTOM
uint P(hash)(K key) {
#  ifdef HT_KEY_ATOMIC
  return ((sizeof(key) <= 4) ? hash_u32(key) : hash_u64(key));
#  elif defined(HT_KEY_MEM)
  // TODO
#  else
#    error Unable to determin which hash function to generate
#  endif
}

bool P(eq)(K a, K b) {
#  ifdef HT_KEY_ATOMIC
  return a == b;
#  elif defined(HT_KEY_MEM)
  return memcmp(a, b) == 0;
#  else
#    error Unable to determin which hash function to generate
#  endif
}
#endif // ifndef HT_KEY_CUSTOM

void P(init)(T* t) {
  t->len = 0;
  t->cap = 8;
  t->graves = 0;
  t->vals = malloc(sizeof(V) * t->cap);
  t->keys = malloc(sizeof(K) * t->cap);
  for (size_t i = 0; i < t->cap; i++)
    MAKE_EMPTY(t->keys[i]);
}

void P(deinit)(T* t) {
  free(t->vals);
  free(t->keys);
}

T* P(alloc)(void) {
  struct P(table)* t = malloc(sizeof(T));
  P(init)(t);
  return t;
}

void P(free)(T* t) {
  P(deinit)(t);
  free(t);
}

#include <stdio.h>
void P(print)(T* t) {
  printf("ht %p %zu %zu %zu\n", t, t->len, t->cap, t->graves);
  for (size_t i = 0; i < t->cap; i++) {
    printf("%d -> %d\n", t->keys[i], t->vals[i]);
  }
}

/**
 * Allocates new containers for keys&values and rehashes all of the values over
 * there (for grave removing and growing).
 */
void P(rehash)(T* t, size_t old_cap) {
  t->graves = 0;
  // I don't think this can be done inplace well
  V* ov = t->vals; // old values
  K* ok = t->keys; // old keys
  t->vals = malloc(sizeof(V) * t->cap);
  t->keys = malloc(sizeof(K) * t->cap);
  for (size_t i = 0; i < t->cap; i++)
    MAKE_EMPTY(t->keys[i]);
  for (size_t i = 0; i < old_cap; i++) {
    K* b = &ok[i];
    if (!IS_EMPTY(*b) && !IS_GRAVE(*b)) { // rehash only non-empty non-graves
      uint hash = P(hash)(*b) % t->cap;
      while (!IS_EMPTY(t->keys[hash])) // walk until we find empty slot
        hash++;
      t->keys[hash] = *b; // move
      t->vals[hash] = ov[i];
    }
  }
  free(ov);
  free(ok);
}

void P(maybe_grow)(T* t) {
  if (t->len > HT_MAX_DENSITY * t->cap) {
    t->cap *= 2;
    P(rehash)(t, t->cap / 2);
  }
}

/**
 * Internal. Looks up the given key and returns its index
 */
size_t P(_get_key_index)(T* t, K k, bool* new) {
  for (size_t i = P(hash)(k) % t->cap;; i = (i + 1) % t->cap) {
    K* b = &t->keys[i];
    if (IS_GRAVE(*b)) {
      continue;
    } else if (IS_EMPTY(*b)) {
      *new = true;
      return i;
    } else if (P(eq)(*b, k)) {
      return i;
    } else {
      continue;
    }
  }
}

/**
 * Finds a value with given key and returns a pointer to it. Returns NULL if the
 * key is not present
 */
V* P(lookup)(T* t, K k) {
  bool new = false;
  size_t i = P(_get_key_index)(t, k, &new);
  return new ? NULL : &t->vals[i];
}

/**
 * Inserts a new key-value pair. If the key is already present returns false.
 * Otherwise returns true
 */
bool P(insert)(T* t, K k, V v) {
  bool new = false;
  size_t i = P(_get_key_index)(t, k, &new);
  if (!new)
    return false;
  t->keys[i] = k;
  t->vals[i] = v;
  t->len++;
  P(maybe_grow)(t);
  return true;
}

/**
 * Updates the value behind the given key.
 * Inserts a new key-value pair if needed.
 */
void P(update)(T* t, K k, V v) {
  bool new = false;
  size_t i = P(_get_key_index)(t, k, &new);
  if (new) {
    t->keys[i] = k;
    t->vals[i] = v;
    P(maybe_grow)(t);
  } else {
    t->vals[i] = v;
  }
}

/**
 * Removes a key.
 */
V P(remove)(T* t, K k, bool* b) {
  bool new = false;
  size_t i = P(_get_key_index)(t, k, &new);
  if (new) {
    *b = false;
    return 0;
  }
  V v = t->vals[i];
  MAKE_GRAVE(t->keys[i]);
  t->graves++;
  *b = true;
  return v;
}

bool P(contains)(T* t, K k) {
  bool new = false;
  P(_get_key_index)(t, k, &new);
  return !new;
}

#undef P
#undef IS_GRAVE
#undef IS_EMPTY
#undef MAKE_GRAVE
#undef MAKE_EMPTY

#undef HT_VAL
#undef HT_KEY
#undef HT_PREFIX
#undef HT_KEY_ATOMIC
#undef HT_KEY_MEM
#undef HT_KEY_STR
#undef HT_KEY_STRPTR
#undef HT_KEY_CUSTOM
#undef HT_WANT_FIND
#undef HT_WANT_LOOKUP
#undef HT_WANT_NEW

