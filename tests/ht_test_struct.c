struct key {
  int state;
  int k;
};

struct val {
  long v;
};

#define HT_KEY struct key
#define HT_VAL struct val
#define HT_PREFIX test
#define HT_KEY_ATOMIC

#define HT_KEY_EMPTY -1
#define HT_KEY_GRAVE -2

#include "ht.h"

int main() {
  struct test_table t;
  test_init(&t);
  int size = 100;

  // check that we can insert values
  for (int i = 0; i < size; i++) {
    assert(test_insert(&t, (struct key){.state = 0, .k = 1},
                       (struct val){.v = 3}));
  }

  // check that they values are there
  for (int i = 0; i < size; i++) {
    assert(test_contains(&t, i));
  }

  // check that we can remove values
  for (int i = 0; i < size; i++) {
    if (i % 2) {
      bool b = false;
      int pop = test_remove(&t, i, &b);
      assert(b);
      assert(pop == 1);
    } else {
      test_update(&t, i, 2);
    }
  }

  // check that only every other key is still there
  for (int i = 0; i < size; i++) {
    if (i % 2) {
      assert(!test_contains(&t, i));
    } else {
      assert(*test_lookup(&t, i) == 2);
    }
  }

  test_print(&t);
  test_deinit(&t);
}

