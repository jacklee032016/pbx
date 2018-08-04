/*
* $Id: digest.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

/* 
*RFC 2617                  HTTP Authentication                  June 1999
*
*
*  Full Copyright Statement
*
*   Copyright (C) The Internet Society (1999).  All Rights Reserved.
*
*   This document and translations of it may be copied and furnished to
*   others, and derivative works that comment on or otherwise explain it
*   or assist in its implementation may be prepared, copied, published
*   and distributed, in whole or in part, without restriction of any
*   kind, provided that the above copyright notice and this paragraph are
*   included on all such copies and derivative works.  However, this
*   document itself may not be modified in any way, such as by removing
*   the copyright notice or references to the Internet Society or other
*   Internet organizations, except as needed for the purpose of
*   developing Internet standards in which case the procedures for
*   copyrights defined in the Internet Standards process must be
*   followed, or as required to translate it into languages other than
*   English.
 
*   The limited permissions granted above are perpetual and will not be
*   revoked by the Internet Society or its successors or assigns.
 
*   This document and the information contained herein is provided on an
*   "AS IS" basis and THE INTERNET SOCIETY AND THE INTERNET ENGINEERING
*   TASK FORCE DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING
*   BUT NOT LIMITED TO ANY WARRANTY THAT THE USE OF THE INFORMATION
*   HEREIN WILL NOT INFRINGE ANY RIGHTS OR ANY IMPLIED WARRANTIES OF
*   MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE. 
*/

#include "digest.hxx"


/*
#define HASHLEN 16
typedef char HASH[HASHLEN];
#define HASHHEXLEN 32
typedef char HASHHEX[HASHHEXLEN+1];
#define IN
#define OUT
 
 
 
#include <stdio.h>
#include <global.h>
#include <string.h>
#include "vmd5.h"   
#include "support.hxx"
*/


/* calculate H(A1) as per HTTP Digest spec */
void DigestCalcHA1(
    IN char * pszAlg,
    IN char * pszUserName,
    IN char * pszRealm,
    IN char * pszPassword,
    IN char * pszNonce,
    IN char * pszCNonce,
    OUT HASHHEX SessionKey
);

/* calculate request-digest/response-digest as per HTTP Digest spec */
void DigestCalcResponse(
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



void CvtHex(
    IN HASH Bin,
    OUT HASHHEX Hex
)
{
    unsigned short i;
    unsigned char j;

    for (i = 0; i < HASHLEN; i++)
    {
        j = (Bin[i] >> 4) & 0xf;
        if (j <= 9)
            Hex[i*2] = (j + '0');
        else
            Hex[i*2] = (j + 'a' - 10);
        j = Bin[i] & 0xf;
        if (j <= 9)
            Hex[i*2 + 1] = (j + '0');
        else
            Hex[i*2 + 1] = (j + 'a' - 10);
    }
    Hex[HASHHEXLEN] = '\0';
}

/* calculate H(A1) as per spec */
void DigestCalcHA1(
    IN char * pszAlg,
    IN char * pszUserName,
    IN char * pszRealm,
    IN char * pszPassword,
    IN char * pszNonce,
    IN char * pszCNonce,
    OUT HASHHEX SessionKey
)
{
    struct MD5Context Md5Ctx;
    HASH HA1;

    MD5Init(&Md5Ctx);
    MD5Update(&Md5Ctx, (unsigned char*)(pszUserName), strlen(pszUserName));
    MD5Update(&Md5Ctx, (unsigned char*)(":"), 1);
    MD5Update(&Md5Ctx, (unsigned char*)(pszRealm), strlen(pszRealm));
    MD5Update(&Md5Ctx, (unsigned char*)(":"), 1);
    MD5Update(&Md5Ctx, (unsigned char*)(pszPassword), strlen(pszPassword));
    MD5Final((unsigned char*)(HA1), &Md5Ctx);
    if (strcmp(pszAlg, "md5-sess") == 0)
    {
        MD5Init(&Md5Ctx);
        MD5Update(&Md5Ctx, (unsigned char*)(HA1), HASHLEN);
        MD5Update(&Md5Ctx, (unsigned char*)(":"), 1);
        MD5Update(&Md5Ctx, (unsigned char*)(pszNonce), strlen(pszNonce));
        MD5Update(&Md5Ctx, (unsigned char*)(":"), 1);
        MD5Update(&Md5Ctx, (unsigned char*)(pszCNonce), strlen(pszCNonce));
        MD5Final((unsigned char*)(HA1), &Md5Ctx);
    }
    CvtHex(HA1, SessionKey);
}

/* calculate request-digest/response-digest as per HTTP Digest spec */
void DigestCalcResponse(
    IN HASHHEX HA1,            /* H(A1) */
    IN char * pszNonce,        /* nonce from server */
    IN char * pszNonceCount,   /* 8 hex digits */
    IN char * pszCNonce,       /* client nonce */
    IN char * pszQop,          /* qop-value: "", "auth", "auth-int" */
    IN char * pszMethod,       /* method from the request */
    IN char * pszDigestUri,    /* requested URL */
    IN HASHHEX HEntity,        /* H(entity body) if qop="auth-int" */
    OUT HASHHEX Response      /* request-digest or response-digest */
)
{
    struct MD5Context Md5Ctx;
    HASH HA2;
    HASH RespHash;
    HASHHEX HA2Hex;

    // calculate H(A2)
    MD5Init(&Md5Ctx);
    MD5Update(&Md5Ctx, (unsigned char*)(pszMethod), strlen(pszMethod));
    MD5Update(&Md5Ctx, (unsigned char*)(":"), 1);
    MD5Update(&Md5Ctx, (unsigned char*)(pszDigestUri), strlen(pszDigestUri));
    if (strcmp(pszQop, "auth-int") == 0)
    {
        MD5Update(&Md5Ctx, (unsigned char*)(":"), 1);
        MD5Update(&Md5Ctx, (unsigned char*)(HEntity), HASHHEXLEN);
    }
    MD5Final((unsigned char*)(HA2), &Md5Ctx);
    CvtHex(HA2, HA2Hex);

    // calculate response
    MD5Init(&Md5Ctx);
    MD5Update(&Md5Ctx, (unsigned char*)(HA1), HASHHEXLEN);
    MD5Update(&Md5Ctx, (unsigned char*)(":"), 1);
    MD5Update(&Md5Ctx, (unsigned char*)(pszNonce), strlen(pszNonce));
    MD5Update(&Md5Ctx, (unsigned char*)(":"), 1);
    if (*pszQop)
    {
        MD5Update(&Md5Ctx, (unsigned char*)(pszNonceCount), strlen(pszNonceCount));
        MD5Update(&Md5Ctx, (unsigned char*)(":"), 1);
        MD5Update(&Md5Ctx, (unsigned char*)(pszCNonce), strlen(pszCNonce));
        MD5Update(&Md5Ctx, (unsigned char*)(":"), 1);
        MD5Update(&Md5Ctx, (unsigned char*)(pszQop), strlen(pszQop));
        MD5Update(&Md5Ctx, (unsigned char*)(":"), 1);
    }
    MD5Update(&Md5Ctx, (unsigned char*)(HA2Hex), HASHHEXLEN);
    MD5Final((unsigned char*)(RespHash), &Md5Ctx);
    CvtHex(RespHash, Response);
}


#if 0
void main(int argc, char ** argv)
{

    char * pszNonce = "dcd98b7102dd2f0e8b11d0f600bfb0c093";
    char * pszCNonce = "0a4f113b";
    char * pszUser = "Mufasa";
    char * pszRealm = "testrealm@host.com";
    char * pszPass = "Circle Of Life";
    char * pszAlg = "md5";
    char szNonceCount[9] = "00000001";
    char * pszMethod = "GET";
    char * pszQop = "auth";
    char * pszURI = "/dir/index.html";
    HASHHEX HA1;
    HASHHEX HA2 = "";
    HASHHEX Response;

    DigestCalcHA1(pszAlg, pszUser, pszRealm, pszPass, pszNonce,
                  pszCNonce, HA1);
    DigestCalcResponse(HA1, pszNonce, szNonceCount, pszCNonce, pszQop,
                       pszMethod, pszURI, HA2, Response);
    printf("Response = %s\n", Response);
}


#endif
