#include "test.h"
#include "ar.h"

void test_init_free() {
	int* a;
	arinit(a);
	escape(a);
	arfree(a);
}

void test_arpush(int iter) {
	int* a;
	arinit(a);

	for (int i = 0; i < iter; i++) {
		arpush(a, i*2);
		assert(arcap(a) > arlen(a));
	}

	for (int i = 0; i < iter; i++)
		if (a[i] != i*2)
			exit(1);

	escape(a);
	arfree(a);
}

void test_arpushm(int iter) {
	int* a;
	arinit(a);

	int* s = arpushm(a, iter);

	assert(arcap(a) >= arlen(a));
	assert(arlen(a) == iter);

	for (int i = 0; i < iter; i++)
		s[i] = i * 3;

	for (int i = 0; i < iter; i++)
		assert(s[i] == i * 3);

	escape(a);
	arfree(a);
}

int main() {
	int iter = 512;
	test_init_free();
	test_arpush(iter);
	test_arpushm(iter);
}

