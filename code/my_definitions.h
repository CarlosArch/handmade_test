#ifndef _MYDEFS_
#define _MYDEFS_
#include <stdint.h>

#define internal    static
#define persistent  static
#define global      static

typedef int8_t   int8;
typedef int16_t  int16;
typedef int32_t  int32;
typedef int64_t  int64;
typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

/* Pixel stuff */
typedef struct PIXEL {
    uint8 blue;
    uint8 green;
    uint8 red;
    uint8 alpha;
} PIXEL;

#define BYTES_PER_PIXEL 4
#endif