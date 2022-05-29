#include "ar.h"
#include "test.h"

void test_init_free() {
  int* a;
  arinit(a);
  escape(a);
  arfree(a);
}

void test_arpush(size_t iter) {
  int* a;
  arinit(a);

  for (size_t i = 0; i < iter; i++) {
    arpush(a, i * 2);
    assert(arcap(a) > arlen(a));
  }

  for (size_t i = 0; i < iter; i++)
    if (a[i] != i * 2)
      __builtin_trap();

  escape(a);
  arfree(a);
}

void test_arpushm(size_t iter) {
  int* a;
  arinit(a);

  int* s = arpushm(a, iter);

  assert(arcap(a) >= arlen(a));
  assert(arlen(a) == iter);

  for (size_t i = 0; i < iter; i++)
    s[i] = i * 3;

  for (size_t i = 0; i < iter; i++)
    assert(s[i] == i * 3);

  escape(a);
  arfree(a);
}

void test_foreach(size_t iter) {
  int* a;
  arinit(a);
  int* s = arpushm(a, iter); // push(=make space for) multiple values

  for (size_t i = 0; i < iter; i++)
    s[i] = 3;

  int v;
  arforev(a, v) { // foreach by value. v is value
    assert(v == 3);
  }

  arforei(a, i) { // foreach by index. i is index
    assert(a[i] == 3);
  }

  escape(a); // optimizer hack so the test doesn't get opmitized away
  arfree(a);
}

int main() {
  size_t iter = 512;
  test_init_free();
  test_arpush(iter);
  test_arpushm(iter);
  test_foreach(iter);
}

