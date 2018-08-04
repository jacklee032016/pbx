/*
* $Log: SipBasic.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipBasic.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"
#include "SipBasic.hxx"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "cpLog.h"
#include "support.hxx"
#include "Base64.hxx"

using namespace Assist;

SipBasic::SipBasic()
{
}


SipBasic::~SipBasic()
{
}


Data SipBasic::formSIPBasic( const Data& user, const Data& pwd ) const
{
    //user:pwd- base64 encoded---RFC 2617


    unsigned char userbuf[1024];
    int userlen;


    unsigned char pwdbuf[1024];
    int pwdlen;

    unsigned char colonbuf[1024];
    int colonlen;


    //convert char* to unsigned char*.

    pwdlen = convertToUnsigned(pwd, pwdbuf);

    colonlen = convertToUnsigned(":", colonbuf);

    userlen = convertToUnsigned(user, userbuf);


    unsigned int concatValuelen;

    //malloc concatValue here.

    unsigned char* concatValue 
	= (unsigned char*)(malloc(pwdlen + colonlen + userlen + 1));

    Data basicCookie;

    if (concatValue)
    {
        memcpy(concatValue, userbuf, userlen);
        concatValuelen = userlen;

        memcpy(concatValue + concatValuelen, colonbuf, colonlen);
        concatValuelen += colonlen;

        memcpy(concatValue + concatValuelen, pwdbuf, pwdlen);
        concatValuelen += pwdlen;

        concatValue[concatValuelen] = '\0';

        //convert digesconcatenated value to base 64

        //allocate 2*concatValuelen.
        unsigned char* encodedBuf = (unsigned char*)(malloc(concatValuelen * 2));

        unsigned int encodedBuflen;


        if (encodedBuf)
        {
            int ret 
		= Assist::Base64::encode((unsigned char*)encodedBuf, 
					&encodedBuflen, 
					(unsigned char*)concatValue, 
					concatValuelen);

            if (ret > 0)
            {
                basicCookie = (char*)(encodedBuf);
            }

            free(encodedBuf);
        }
        free(concatValue);
    }

    return basicCookie;
} 
