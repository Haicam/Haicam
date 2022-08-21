#ifndef SDTYPE_H
#define SDTYPE_H
#include <stdint.h>

typedef  unsigned char     BYTE;

#ifndef FALSE
#define FALSE			   false
#endif

#ifndef TRUE
#define TRUE               true
#endif


//
// One byte signed type
//
typedef char         CHAR;

//
// One byte unsigned type
//
typedef unsigned char       UCHAR;

//
// One byte signed type
//
typedef int8_t        INT8;

//
// One byte unsigned type
//
typedef uint8_t       UINT8;

//
// Two bytes signed type
//
typedef int16_t        INT16;

//
// Two bytes unsigned type
//
typedef uint16_t      UINT16;

//
// Four bytes signed type
//
typedef int32_t         INT32;

//
// Four bytes unsigned type
//
typedef uint32_t        UINT32;

//
// Eight bytes signed type
//
typedef int64_t   INT64;

//
// Eight bytes unsigned type
//
typedef uint64_t  UINT64;

//
//double type
//
typedef double  DOUBLE;

//
//float type
//
typedef float  FLOAT;

#endif




