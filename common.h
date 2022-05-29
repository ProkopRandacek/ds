#ifndef DS_COMMON_H
#define DS_COMMON_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

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

#define CHECK_PTR_TYPE(x, type) ((x) - (type)(x) + (type)(x))
#define OFFSETOF(s, i) ((uint)offsetof(s, i))
#define SKIP_BACK(s, i, p) ((s*)((char*)p - OFFSETOF(s, i)))

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define ABS(x) ((x) < 0 ? -(x) : (x))

#define GLUE(x,y) x##y
#define GLUE_(x,y) x##_##y
#define GLUE_EXPANDED(x,y) GLUE(x,y)
#define GLUE_EXPANDED_(x,y) GLUE_(x,y)

#define clzl(x) __builtin_clzl(x)
#define clz(x) __builtin_clz(x)
#define ctzl(x) __builtin_ctzl(x)
#define ctz(x) __builtin_ctz(x)
#define popc(x) __builtin_popcount(x)

#endif
