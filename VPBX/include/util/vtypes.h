#ifndef VTYPES_H_
#define VTYPES_H_

/*
* $Id: vtypes.h,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#if defined(__vxworks)

/* VxWorks types */
#include <time.h>
#include <sys/times.h>

#include "types/vxTypesOld.h"

#ifndef u_int32_t
typedef UINT32 u_int32_t;
#endif

#ifndef UWORD32
typedef UINT32 UWORD32;
#endif

#ifndef u_int16_t
typedef UINT16 u_int16_t;
#endif

#ifndef u_int8_t
typedef UINT8 u_int8_t;
#endif

#elif defined(__sparc)
#include <inttypes.h>

/* typedef unsigned char u_int8_t; */
typedef uint8_t u_int8_t;
typedef uint16_t u_int16_t;
typedef uint32_t u_int32_t;

#elif defined(__svr4__)

#include <pthread.h>

#ifndef u_int32_t
typedef uint32_t u_int32_t;
#endif

#ifndef u_int8_t
typedef uint8_t u_int8_t;
#endif

#elif defined (WIN32)
#include "windows.h"

#ifndef u_int32_t
typedef UINT32  u_int32_t;
#endif

#ifndef u_int16_t
typedef USHORT  u_int16_t;
#endif

#ifndef u_int8_t
typedef UCHAR  u_int8_t;
#endif

#ifndef socklen_t
typedef INT32 socklen_t;
#endif

#ifndef int64_t
typedef INT64 int64_t;
#endif
#else
#include <sys/types.h>
#endif
/* endif machine type */


#endif
