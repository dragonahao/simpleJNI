#ifndef _COMM_DEF_H_
#define _COMM_DEF_H_

#include <stdint.h>

#ifndef FALSE
#define FALSE               0
#endif

#ifndef TRUE
#define TRUE                1
#endif

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef OPTIONAL
#define OPTIONAL
#endif

#define BUF_SIZE_1024 1024

#define BUF_SIZE_4096 4096

/**
 * 计算数组元素个数。只能用于数组，绝不能用于指针。
 */
#define array_size(arr) (sizeof(arr)/sizeof((arr)[0]))

#ifdef NDEBUG
# define RETURN_VOID()           do { (void)(pResult); return; } while(0)
#else
# define RETURN_VOID()           do { pResult->i = 0xfefeabab; return; }while(0)
#endif

/**
 * 将目标内存清零。
 */
#define ZeroMemory(Destination,Length) memset((Destination),0,(Length))

typedef uint8_t             u1;
typedef uint16_t            u2;
typedef uint32_t            u4;
typedef uint64_t            u8;
typedef int8_t              s1;
typedef int16_t             s2;
typedef int32_t             s4;
typedef int64_t             s8;

#endif
