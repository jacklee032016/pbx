#if 0
#ifndef VMD5_H
#define VMD5_H

static const char* const vmd5HeaderVersion =
    "$Id: vmd5.h,v 1.1.1.1 2006/11/29 09:28:02 lizhijie Exp $";

/*
 * This is the header file for the MD5 message-digest algorithm.
 * The algorithm is due to Ron Rivest.  This code was
 * written by Colin Plumb in 1993, no copyright is claimed.
 * This code is in the public domain; do with it what you wish.
 *
 * Equivalent code is available from RSA Data Security, Inc.
 * This code has been tested against that, and is equivalent,
 * except that you don't need to include two pages of legalese
 * with every copy.
 *
 * To compute the message digest of a chunk of bytes, declare an
 * MD5Context structure, pass it to MD5Init, call MD5Update as
 * needed on buffers full of bytes, and then call MD5Final, which
 * will fill a supplied 16-byte array with the digest.
 *
 * Changed so as no longer to depend on Colin Plumb's `usual.h'
 * header definitions; now uses stuff from dpkg's config.h
 *  - Ian Jackson <ijackson@nyx.cs.du.edu>.
 * Still in the public domain.
 */


#include "vtypes.h"
#ifdef __cplusplus
extern "C"
{
#endif

#define md5byte unsigned char

    struct MD5Context
    {
        u_int32_t buf[4];
        u_int32_t bytes[2];
        u_int32_t in[16];
    };

    void MD5Init(struct MD5Context *context);
    void MD5Update(struct MD5Context *context, md5byte const *buf, unsigned len);
    void MD5Final(unsigned char digest[16], struct MD5Context *context);
    void MD5Transform(u_int32_t buf[4], u_int32_t const in[16]);

#ifdef __cplusplus
}
#endif

/* !MD5_H */
#endif

#endif

#ifndef VMD5_H
#define VMD5_H

static const char* const vmd5HeaderVersion =
    "$Id: vmd5.h,v 1.1.1.1 2006/11/29 09:28:02 lizhijie Exp $";

/*
 * This is the header file for the MD5 message-digest algorithm.
 * The algorithm is due to Ron Rivest.  This code was
 * written by Colin Plumb in 1993, no copyright is claimed.
 * This code is in the public domain; do with it what you wish.
 *
 * Equivalent code is available from RSA Data Security, Inc.
 * This code has been tested against that, and is equivalent,
 * except that you don't need to include two pages of legalese
 * with every copy.
 *
 * To compute the message digest of a chunk of bytes, declare an
 * MD5Context structure, pass it to MD5Init, call MD5Update as
 * needed on buffers full of bytes, and then call MD5Final, which
 * will fill a supplied 16-byte array with the digest.
 *
 * Changed so as no longer to depend on Colin Plumb's `usual.h'
 * header definitions; now uses stuff from dpkg's config.h
 *  - Ian Jackson <ijackson@nyx.cs.du.edu>.
 * Still in the public domain.
 */


#include "vtypes.h"
#ifdef __cplusplus
extern "C"
{
#endif

#define md5byte unsigned char
typedef unsigned long int UINT4;
typedef unsigned char* POINTER;


    struct MD5Context
    {
        UINT4 buf[4];		/* state (ABCD) */
        UINT4 bytes[2];		/* number of bits, modulo 2^64 (lsb first) */
        unsigned char in[64];	/* input buffer */
    };


    void MD5Init(struct MD5Context *context);
    void MD5Update( struct MD5Context *context,  unsigned char *input, unsigned int inputLen);
    void MD5Final(unsigned char digest[16], struct MD5Context *context);
    void MD5Transform(UINT4 state[4], unsigned char block[64]);
   /* void MD5Size();*/
    void MD5_memcpy(unsigned char*, unsigned char*,unsigned int );
    void MD5_memset (unsigned char*, int, unsigned int);
    void Encode (unsigned char *output, UINT4 *input,   unsigned int len);
    void Decode (UINT4 *output,  unsigned char *input, unsigned int len);
	
#ifdef __cplusplus
}
#endif

/* !MD5_H */
#endif
