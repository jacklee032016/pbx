/*
* $Id: EndianVerify.cxx,v 1.2 2007/03/02 07:29:48 lizhijie Exp $
*/

#include "rtpTypes.h"
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include "Verify.hxx"

char* a = "\x80";

char packet[] = "\x80\x00\x15\x42\x06\x49\x8B\xC0\xA7\xE6\xC2\x94\x7E\x7E\x7F\x7E\x7E\x7E\x7E\x7E\x7E\x7E\x7E\x7E\x7F\x7F\x7F\x7F\x7F\x7E\x7F\x7F\x7F\xFF\xFF\x7F\xFF\xFF\xFF\x7F\x7F\x7E\x7E\x7E\x7E\x7E\x7E\x7F\xFE\xFE\xFE\xFE\xFE\xFF\xFF\xFE\xFF\x7F\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x7E\x7E\x7E\x7E\x7E\x7E\x7E\x7F\x7F\x7F\x7F\x7E\x7F\x7F\x7F\x7F\xFF\xFE\xFE\xFE\xFF\xFE\xFE\x7F\xFF\xFF\x7F\x7F\x7F\xFF\xFF\xFF\xFE\xFE\xFF\x7F\x7F\x7E\x7E\x7E\x7E\x7E\x7F\x7F\x7F\x7F\xFE\xFE\xFF\xFF\xFE\xFF\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\xFE\xFE\xFF\xFE\xFE\xFE\xFE\xFE\xFE\xFE\xFF\x7F\x7F\x7F\x7E\x7E\x7E\x7E\x7E\x7E\x7E\x7E\x7F\x7F\x7F\xFF\xFF\x7F\xFF\xFE\xFF\xFF\xFF\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F";


#if 0
struct Evil
{
    /// protocal version
    unsigned char version:    2;
    /// padding flag - for encryption
    unsigned char padding:    1;
    /// header extension flag
    unsigned char extension:  1;
    /// csrc count
    unsigned char count:      4;


    

    /// marker bit - for profile
    unsigned char marker:      1;
    /// payload type
    unsigned char type:        7;



    /// sequence number of this packet -- this needs to be in network
    /// byte order
    unsigned short sequence : 16;
};
#endif

void output(char c)
{
    unsigned int i = c;
    printf("%x\n", i % 0xff);
}


typedef RtpHeader Foo;


int main()
{
    // this is needed to get things into word alignment on sun boxes.
    char* y = new char[32];
    memcpy(y, packet, 32);


    Foo* x = reinterpret_cast<Foo*>(y);

    test_verify(x->version == 0x2);
    test_verify(ntohs(x->sequence) == 0x1542);


#if 0
    printf("%x\n", x->version);
    printf("%x\n", ntohs(x->sequence));
 

    printf("%x %x\n", x->type, x->marker);
    printf("%x %x %x %x\n", x->version, x->padding, x->extension, x->count);
    printf("%x %x\n", x->version, x->count);


    output(packet[0]);
    output(packet[1]);
    output(packet[2]);
    output(packet[3]);
#endif

    return test_return_code(2);
}


