#ifndef DS_COMMON_H
#define DS_COMMON_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

typedef uint8_t byte;
typedef uint8_t u8;
typedef int8_t s8;
typedef uint16_t u16;
typedef int16_t s16;
typedef uint32_t u32;
typedef int32_t s32;
typedef uint64_t u64;
typedef int64_t s64;
typedef unsigned int uint;

#define ds_offsetof(s, i) ((uint)offsetof(s, i))
#define ds_skip_back(s, i, p) ((s*)((char*)p - offsetof(s, i)))

#define ds_min(a, b) (((a) < (b)) ? (a) : (b))
#define ds_max(a, b) (((a) > (b)) ? (a) : (b))
#define ds_abs(x) ((x) < 0 ? -(x) : (x))

#define ds_glue(x, y) x##y
#define ds_glue_(x, y) x##_##y
#define ds_glue_expanded(x, y) glue(x, y)
#define ds_glue_expanded_(x, y) glue_(x, y)

#define clzl(x) __builtin_clzl(x)
#define clz(x) __builtin_clz(x)
#define ctzl(x) __builtin_ctzl(x)
#define ctz(x) __builtin_ctz(x)
#define popc(x) __builtin_popcount(x)

#define likely(x) __builtin_expect((x), 1)
#define unlikely(x) __builtin_expect((x), 0)

#endif
