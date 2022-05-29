# ds

Some generic data structure headers.
See [`tests/`](./tests/) for example usages

## ar.h
Growing array.

## ht.h
Hash table generator.

## x.h
`malloc` and `realloc` can return `NULL` when there is no memory available. This header implements `xmalloc` and `xrealloc` (and `xfree` for completness) that die when `malloc`/`realloc` fails.

