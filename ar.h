#include "common.h"

struct ar_head {
  size_t len, cap;
  byte elms[]; // this is where the data lives. byte is just a placeholder and
               // does not represent the actual type
};

#define arinit(p)                                                              \
  do {                                                                         \
    struct ar_head* _h = malloc(sizeof(*_h) + sizeof(*p) * 2);                 \
    _h->len = 0;                                                               \
    _h->cap = 1;                                                               \
    p = (typeof(p))&_h->elms;                                                  \
  } while (0)

#define arfree(p) free(SKIP_BACK(struct ar_head, elms, p))

#define arlen(p)                                                               \
  ({                                                                           \
    struct ar_head* _h = SKIP_BACK(struct ar_head, elms, p);                   \
    _h->len;                                                                   \
  })

#define arcap(p)                                                               \
  ({                                                                           \
    struct ar_head* _h = SKIP_BACK(struct ar_head, elms, p);                   \
    _h->cap;                                                                   \
  })

#define arpushm(p, n)                                                          \
  ({                                                                           \
    struct ar_head* _h = SKIP_BACK(struct ar_head, elms, p);                   \
    size_t _l = _h->len;                                                       \
    _h->len += n;                                                              \
    if (_h->len >= _h->cap) {                                                  \
      _h->cap = MAX(_h->cap * 2, _h->len);                                     \
      _h = realloc(_h, sizeof(*_h) + sizeof(*p) * _h->cap);                    \
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
    struct ar_head* _h = SKIP_BACK(struct ar_head, elms, p);                   \
    size_t _c = _h->len + n;                                                   \
    if (_c >= _h->cap) {                                                       \
      _h->cap = _c;                                                            \
      _h = realloc(_h, sizeof(*_h) + sizeof(*p) * _h->cap);                    \
      p = (typeof(p))&_h->elms;                                                \
    }                                                                          \
  })

// foreach by value
#define arforev(p, v)                                                          \
  for (size_t i = 0, end = arlen(p); ({                                        \
         v = (i < end) ? p[i] : 0;                                             \
         i < end;                                                              \
       });                                                                     \
       i++)

// foreach by index
#define arforei(p, i) for (size_t i = 0, end = arlen(p); i < end; i++)

