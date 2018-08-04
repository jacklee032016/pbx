/*
 * $Id: digest.hxx,v 1.2 2007/03/01 20:04:29 lizhijie Exp $
 */


#include <stdio.h>
#include <global.h>
#include <string.h>
#include "vmd5.h"
#include "support.hxx"


#define HASHLEN 16
typedef char HASH[HASHLEN];
#define HASHHEXLEN 32
typedef char HASHHEX[HASHHEXLEN + 1];
#ifdef WIN32
#undef IN
#endif
#define IN const
#define OUT


/* calculate H(A1) as per HTTP Digest spec */
extern void
    DigestCalcHA1(
        IN char * pszAlg,
        IN char * pszUserName,
        IN char * pszRealm,
        IN char * pszPassword,
        IN char * pszNonce,
        IN char * pszCNonce,
        OUT HASHHEX SessionKey
    );

/* calculate request-digest/response-digest as per HTTP Digest spec */
extern void
    DigestCalcResponse(
        IN HASHHEX HA1,            /* H(A1) */
        IN char * pszNonce,        /* nonce from server */
        IN char * pszNonceCount,   /* 8 hex digits */
        IN char * pszCNonce,       /* client nonce */
        IN char * pszQop,          /* qop-value: "", "auth", "auth-int" */
        IN char * pszMethod,       /* method from the request */
        IN char * pszDigestUri,    /* requested URL */
        IN HASHHEX HEntity,        /* H(entity body) if qop="auth-int" */
        OUT HASHHEX Response      /* request-digest or response-digest */
    );



extern void
    CvtHex(
        IN HASH Bin,
        OUT HASHHEX Hex
    );
