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
 * HT_WANT_PRINT - Create a debug print function
 *
 * #### HT_MULTIKEY
 * Allows you to alter number of arguments that all functions take as key.
 * For example it might be desired to have two ints as a key but creating
 * a struct for it is annoying.
 * Internally, a struct is created to store the keys.
 *
 * HT_MULTIKEY has to have a value in format
 *
 * ### Functions
 *
 * Function | Description
 * ---------|----
 * init     | Init a table
 * deinit   | Free memory used by a table
 * alloc    | Alloc + init a table
 * free     | Free memory used by a table and the table itself
 *
 * lookup   | Try to find a value under a key.
 * insert   | Try to insert a new key-value pair.
 * update   | Update value under a key. Create key if needed.
 * delete   | Delete a key-value pair if it exists.
 */

#if defined(HT_MULTIKEY) && defined(HT_KEY)
#  error You cant define both HT_MULTIKEY and HT_KEY
#endif

#if !defined(HT_MULTIKEY) && !defined(HT_KEY)
#  error You have to define either HT_KEY or HT_MULTIKEY
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

#ifndef HT_FUNC_ATTR
#  define HT_FUNC_ATTR
#endif

#define P(x) ds_glue_expanded_(HT_PREFIX, x)

#ifdef HT_MULTIKEY
struct P(key) {
  HT_MULTIKEY(, ;)
};
#  define KARG HT_MULTIKEY(, ds_comma)
#  define KARGPASS HT_MULTIKEY_NAMES(, )
#  define HT_KEY P(key)
#else
#  define KARG K k
#  define KARGPASS k
#endif
// ended here, checks about defined macros combined with other macros above.

// shortcuts
#define K HT_KEY
#define V HT_VAL
#define IS_GRAVE(x) P(eq)(HT_KEY_GRAVE, x)
#define IS_EMPTY(x) P(eq)(HT_KEY_EMPTY, x)

#define MAKE_GRAVE(x) x = HT_KEY_GRAVE
#define MAKE_EMPTY(x) x = HT_KEY_EMPTY

#define T struct P(table)

struct P(table) {
  size_t len;    // number of elements in table
  size_t cap;    // capacity of the key&value containers
  size_t graves; // number of graves in the table
  V* vals;
  K* keys;
#ifdef HT_TABLE_EXTRA_VARS
  HT_TABLE_EXTRA_VARS
#endif
};

#ifndef HT_KEY_CUSTOM
HT_FUNC_ATTR uint P(hash)(K key) {
#  ifdef HT_KEY_ATOMIC
  return ((sizeof(key) <= 4) ? ds_hash_u32(key) : ds_hash_u64(key));
#  elif defined(HT_KEY_MEM)
  return ds_hash_mem(HT_KEY_LEN, &key);
#  elif defined(HT_KEY_HASH)
  return HT_KEY_HASH(key);
#  else
#    error Unable to determin which hash function to generate
#  endif
}

HT_FUNC_ATTR bool P(eq)(K a, K b) {
#  ifdef HT_KEY_ATOMIC
  return a == b;
#  elif defined(HT_KEY_MEM)
  return memcmp(a, b, HT_KEY_LEN) == 0;
#  elif defined(HT_KEY_EQ)
  return HT_KEY_EQ(a, b);
#  else
#    error Unable to determin which hash function to generate
#  endif
}
#endif // ifndef HT_KEY_CUSTOM

HT_FUNC_ATTR void P(init)(T* t) {
  t->len = 0;
  t->cap = 8;
  t->graves = 0;
  t->vals = malloc(sizeof(V) * t->cap);
  t->keys = malloc(sizeof(K) * t->cap);
  for (size_t i = 0; i < t->cap; i++)
    MAKE_EMPTY(t->keys[i]);
}

HT_FUNC_ATTR void P(deinit)(T* t) {
  free(t->vals);
  free(t->keys);
}

HT_FUNC_ATTR T* P(alloc)(void) {
  T* t = malloc(sizeof(T));
  P(init)(t);
  return t;
}

HT_FUNC_ATTR void P(free)(T* t) {
  P(deinit)(t);
  free(t);
}

#ifdef HT_WANT_PRINT
#  include <stdio.h>
HT_FUNC_ATTR void P(print)(T* t) {
  printf("ht @ %p: len=%zu cap=%zu graves=%zu\n", t, t->len, t->cap, t->graves);
  printf("+ - grave, . - empty, # - used\n");
  for (size_t i = 0; i < t->cap; i++) {
    K k = t->keys[i];
    if (IS_GRAVE(k)) {
      putchar('+');
    } else if (IS_EMPTY(k)) {
      putchar('.');
    } else {
      putchar('#');
    }
  }
  putchar('\n');
}
#endif

/**
 * Allocates new containers for keys&values and rehashes all of the values over
 * there (for grave removing and growing).
 *
 * I don't think its possible to do it inplace
 */
HT_FUNC_ATTR void P(rehash)(T* t, size_t old_cap) {
  t->graves = 0;
  V* ov = t->vals; // old values
  K* ok = t->keys; // old keys
  t->vals = malloc(sizeof(V) * t->cap);
  t->keys = malloc(sizeof(K) * t->cap);
  for (size_t i = 0; i < t->cap; i++)
    MAKE_EMPTY(t->keys[i]);
  for (size_t i = 0; i < old_cap; i++) {
    K b = ok[i];
    if (!IS_EMPTY(b) && !IS_GRAVE(b)) { // rehash only non-empty non-graves
      uint hash = P(hash)(b) % t->cap;
      while (!IS_EMPTY(t->keys[hash])) // walk until we find empty slot
        hash = (hash + 1) % t->cap;
      t->keys[hash] = b; // move
      t->vals[hash] = ov[i];
    }
  }
  free(ov);
  free(ok);
}

HT_FUNC_ATTR void P(_maybe_grow)(T* t) {
  if (t->len > HT_MAX_DENSITY * t->cap) {
    t->cap *= 2;
    P(rehash)(t, t->cap / 2);
  }
}

HT_FUNC_ATTR void P(_maybe_clear)(T* t) {
  if (t->graves > HT_MAX_GRAVE * t->cap)
    P(rehash)(t, t->cap);
}

/**
 * Internal. Looks up the given key and returns its index
 */
HT_FUNC_ATTR size_t P(_get_key_index)(T* t, K k, bool* new) {
  for (size_t i = P(hash)(KARGPASS) % t->cap;; i = (i + 1) % t->cap) {
    K b = t->keys[i];
    if (IS_GRAVE(b))
      continue;
    else if (IS_EMPTY(b)) {
      *new = true;
      return i;
    } else if (P(eq)(b, KARGPASS))
      return i;
    else
      continue;
  }
}

/**
 * Finds a value with given key and returns a pointer to it. Returns NULL if the
 * key is not present
 */
HT_FUNC_ATTR V* P(lookup)(T* t, KARG) {
  bool new = false;
  size_t i = P(_get_key_index)(t, KARGPASS, &new);
  return new ? NULL : &t->vals[i];
}

/**
 * Inserts a new key-value pair. If the key is already present returns false.
 * Otherwise returns true
 */
HT_FUNC_ATTR bool P(insert)(T* t, K k, V v) {
  bool new = false;
  size_t i = P(_get_key_index)(t, k, &new);
  if (!new)
    return false;
  t->keys[i] = k;
  t->vals[i] = v;
  t->len++;
  P(_maybe_grow)(t);
  return true;
}

/**
 * Updates the value behind the given key.
 * Inserts a new key-value pair if needed.
 */
HT_FUNC_ATTR void P(update)(T* t, K k, V v) {
  bool new = false;
  size_t i = P(_get_key_index)(t, k, &new);
  if (new) {
    t->keys[i] = k;
    t->vals[i] = v;
    t->len++;
    P(_maybe_grow)(t);
  } else {
    t->vals[i] = v;
  }
}

/**
 * Removes a key.
 */
HT_FUNC_ATTR V P(remove)(T* t, K k, bool* b) {
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
  P(_maybe_clear)(t);
  return v;
}

HT_FUNC_ATTR bool P(contains)(T* t, K k) {
  bool new = false;
  P(_get_key_index)(t, k, &new);
  return !new;
}

#undef P
#undef T

#undef K
#undef V

#undef IS_GRAVE
#undef IS_EMPTY
#undef MAKE_GRAVE
#undef MAKE_EMPTY

#undef HT_KEY
#undef HT_KEY_ATOMIC
#undef HT_KEY_CUSTOM
#undef HT_KEY_EMPTY
#undef HT_KEY_GRAVE
#undef HT_KEY_MEM
#undef HT_KEY_STR
#undef HT_KEY_STRPTR
#undef HT_MAX_DENSITY
#undef HT_MAX_GRAVE
#undef HT_VAL

#undef HT_KEY_LEN

#undef HT_WANT_PRINT
