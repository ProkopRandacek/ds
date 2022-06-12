#define HT_KEY int
#define HT_VAL int
#define HT_PREFIX test
#define HT_KEY_ATOMIC
#define HT_WANT_PRINT

#define HT_KEY_EMPTY -1
#define HT_KEY_GRAVE -2

#include "../ht.h"

int main() {
  struct test_table t;
  test_init(&t);
  // test_print(&t);
  int size = 100;

  // check that we can insert values
  for (int i = 0; i < size; i++)
    assert(test_insert(&t, i, 1));

  // check that they values are there
  for (int i = 0; i < size; i++)
    assert(test_contains(&t, i));

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
  for (int i = 0; i < size; i++)
    if (i % 2)
      assert(!test_contains(&t, i));
    else
      assert(*test_lookup(&t, i) == 2);

  test_rehash(&t, t.cap);
  test_print(&t);

  test_deinit(&t);
}
