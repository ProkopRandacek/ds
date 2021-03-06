#ifndef DS_AR_H
#define DS_AR_H

#include "common.h"

#ifndef ds_realloc
#  define ds_realloc realloc
#endif

struct ar_head {
  size_t len, cap;
  byte elms[]; // this is where the data lives. byte is just a placeholder and
               // does not represent the actual type
};

#define arinit(p)                                                              \
  do {                                                                         \
    struct ar_head* _h = ds_realloc(NULL, sizeof(*_h) + sizeof(*p) * 2);       \
    _h->len = 0;                                                               \
    _h->cap = 1;                                                               \
    p = (typeof(p))&_h->elms;                                                  \
  } while (0)

#define arfree(p)                                                              \
  do {                                                                         \
    /* warning is emitted when we dont use realloc's return value */           \
    ds_unused void* _ = ds_realloc(ds_skip_back(struct ar_head, elms, p), 0);  \
  } while (0)

#define arlen(p)                                                               \
  ({                                                                           \
    struct ar_head* _h = ds_skip_back(struct ar_head, elms, p);                \
    _h->len;                                                                   \
  })

#define arcap(p)                                                               \
  ({                                                                           \
    struct ar_head* _h = ds_skip_back(struct ar_head, elms, p);                \
    _h->cap;                                                                   \
  })

#define arpushm(p, n)                                                          \
  ({                                                                           \
    struct ar_head* _h = ds_skip_back(struct ar_head, elms, p);                \
    size_t _l = _h->len;                                                       \
    _h->len += n;                                                              \
    if (_h->len >= _h->cap) {                                                  \
      _h->cap = ds_max(_h->cap * 2, _h->len);                                  \
      _h = ds_realloc(_h, sizeof(*_h) + sizeof(*p) * _h->cap);                 \
      p = (typeof(p))&_h->elms;                                                \
    }                                                                          \
    typeof(*(p))* _c = p + _l;                                                 \
    _c;                                                                        \
  })

#define arpush(p, val)                                                         \
  do {                                                                         \
    *arpushm(p, 1) = val;                                                      \
  } while (0)

#define arreserve(p, n)                                                        \
  ({                                                                           \
    struct ar_head* _h = ds_skip_back(struct ar_head, elms, p);                \
    size_t _c = _h->len + n;                                                   \
    if (_c >= _h->cap) {                                                       \
      _h->cap = _c;                                                            \
      _h = ds_realloc(_h, sizeof(*_h) + sizeof(*p) * _h->cap);                 \
      p = (typeof(p))&_h->elms;                                                \
    }                                                                          \
  })

#define arpop(p)                                                               \
  ({                                                                           \
    struct ar_head* _h = ds_skip_back(struct ar_head, elms, p);                \
    typeof(*(p)) _c = p[--_h->len];                                            \
    _c;                                                                        \
  })

#define arpeek(p)                                                              \
  ({                                                                           \
    struct ar_head* _h = ds_skip_back(struct ar_head, elms, p);                \
    typeof(*(p)) _c = p[_h->len - 1];                                          \
    _c;                                                                        \
  })

// foreach by value
#define arforev(p, v)                                                          \
  for (int _k = 0; !_k;)                                                       \
    for (typeof(*p) v; !_k;)                                                   \
      for (size_t i = 1, end = arlen(p); !_k;)                                 \
        for (_k = 1; ({                                                        \
               if (i == end) {                                                 \
                 break;                                                        \
               } else {                                                        \
                 v = p[i];                                                     \
               }                                                               \
               i < end;                                                        \
             });                                                               \
             i++)

// foreach by index
#define arforei(p, i) for (size_t i = 0, end = arlen(p); i < end; i++)

#endif
