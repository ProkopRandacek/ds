#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>

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

#define CHECK_PTR_TYPE(x, type) ((x)-(type)(x) + (type)(x))
#define OFFSETOF(s, i) ((uint)offsetof(s, i))				/** Offset of item @i from the start of structure @s **/
#define SKIP_BACK(s, i, p) ((s *)((char *)p - OFFSETOF(s, i))) /** Given a pointer @p to item @i of structure @s, return a pointer to the start of the struct. **/

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
#define CLAMP(x,min,max) ({ typeof(x) _t=x; (_t < min) ? min : (_t > max) ? max : _t; })
#define ABS(x) ((x) < 0 ? -(x) : (x))

#define clzl(x)  __builtin_clzl(x)
#define clz(x)   __builtin_clz(x)
#define ctzl(x)  __builtin_ctzl(x)
#define ctz(x)   __builtin_ctz(x)
#define popc(x)  __builtin_popcount(x)

