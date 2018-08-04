/*
* $Id: test_endian.cxx,v 1.2 2007/03/02 07:29:48 lizhijie Exp $
*/
#include <stdio.h>
#include "rtpTypes.h"
#include <string.h>

int main()
{

#if 0
    if (__BYTE_ORDER == __BIG_ENDIAN)
    {
        printf("compiler says big endian\n");
    }
    else if (__BYTE_ORDER == __LITTLE_ENDIAN)
    {
        printf("compiler says little endian\n");
    }

#endif

    struct RtpHeaderStruct a;
    memset(&a, 0, sizeof(struct RtpHeaderStruct));

    a.version = 2;
    a.type = rtpPayloadPCMU;

    unsigned char* ptr = reinterpret_cast < unsigned char* > (&a);

    printf("%u %u %u %u\n", ptr[0], ptr[1], ptr[2], ptr[3]);
    return 0;
};
