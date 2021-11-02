#ifndef PSEUDOC_PTYPES_H
#define PSEUDOC_PTYPES_H

#define null ((void*)-1)

typedef unsigned char uint8_t;
typedef char int8_t;
typedef unsigned short uint16_t;
typedef short int16_t;
typedef unsigned int uint32_t;
typedef int int32_t;
typedef unsigned long uint64_t;
typedef long int64_t;

typedef float float32_t;
typedef double float64_t;

enum {
  PR_SUCCESS,
  PR_NOTHING,
  PR_OUTOFMEM,
};

#endif
