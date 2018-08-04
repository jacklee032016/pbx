/*
 * $Id: SipCHAP.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */

#include "SipCHAP.hxx"
#include "cpLog.h"
#include "support.hxx"
#include "vmd5.h"
#include "digest.hxx"

using namespace Vocal;

Data 
SipCHAP::form_CHAP(const Data& id,
                   const Data& pwd,
                   const Data& challenge)
{
    
    HASHHEX response;
    struct MD5Context Md5Ctx;
    HASH hash;
    char * tmp;

    MD5Init(&Md5Ctx);
    LocalScopeAllocator lo;
    tmp = const_cast<char*>(id.getData(lo));
    MD5Update(&Md5Ctx, (unsigned char*)(tmp), strlen(tmp));

    tmp = const_cast<char*>(pwd.getData(lo));
    MD5Update(&Md5Ctx, (unsigned char*)(tmp), strlen(tmp));

    tmp = const_cast<char*>(challenge.getData(lo));
    MD5Update(&Md5Ctx, (unsigned char*)(tmp), strlen(tmp));

    MD5Final((unsigned char*)(hash), &Md5Ctx);

    CvtHex(hash, response);

    cpDebug(LOG_INFO, "Response = %s\n", response);

    return response;
}
