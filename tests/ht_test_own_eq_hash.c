#include "../hash.h"

struct key {
  int a;
  int b;
};

#define HT_KEY struct key
#define HT_VAL int
#define HT_PREFIX test

#define HT_KEY_EMPTY                                                           \
  (struct key) { .a = -1, .b = 0 }
#define HT_KEY_GRAVE                                                           \
  (struct key) { .a = -2, .b = 0 }

#define HT_KEY_EQ(i, j) ((i).a == (j).a && (i).b == (j).b)
#define HT_KEY_HASH(i) (ds_hash_u32((i).a) ^ ds_hash_u32((i).b))

#define HT_WANT_PRINT

#include "../ht.h"

int main() {
  struct test_table t;
  test_init(&t);
  int size = 100;

  // check that we can insert values
  for (int i = 0; i < size; i++) {
    assert(test_insert(&t, (struct key){.a = i, .b = i * 2}, 1));
  }

  // check that they values are there
  for (int i = 0; i < size; i++) {
    assert(test_contains(&t, (struct key){.a = i, .b = i * 2}));
  }

  // check that we can remove values
  for (int i = 0; i < size; i++) {
    if (i % 2) {
      bool b = false;
      int pop = test_remove(&t, (struct key){.a = i, .b = i * 2}, &b);
      assert(b);
      assert(pop == 1);
    } else {
      test_update(&t, (struct key){.a = i, .b = i * 2}, 2);
    }
  }

  // check that only every other key is still there
  for (int i = 0; i < size; i++) {
    if (i % 2) {
      assert(!test_contains(&t, (struct key){.a = i, .b = i * 2}));
    } else {
      assert(*test_lookup(&t, (struct key){.a = i, .b = i * 2}) == 2);
    }
  }

  test_print(&t);
  test_deinit(&t);
}
