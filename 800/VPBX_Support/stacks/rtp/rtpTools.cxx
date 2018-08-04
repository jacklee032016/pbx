/*
* $Log: rtpTools.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: rtpTools.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"
#include <iostream>
#include <cassert>
#include <stdio.h>
#include <stdlib.h>
#include "rtpTypes.h"
#include "rtpTools.hxx"
#include "cpLog.h"
//#include "vsock.hxx"

/* ----------------------------------------------------------------- */
/* --- Number Functions -------------------------------------------- */
/* ----------------------------------------------------------------- */

/*  32-bit random number     */
u_int32_t generate32 ()
{
    // should be seeded by main program
    return rand();
}


/*  random SRC number        */
RtpSrc generateSRC()
{
    // doesn't check for collision
    RtpSrc src = 0;
    while (src == 0)
        src = generate32();
    return src;
}


/*  prints raw bits of memory */
void printBits (char* packetData, int len)
{
    unsigned short int b;
    unsigned int c = 0;
    cerr << "\n-----------------------------------\n";
    for (int i = 0; i < len; i++)
    {
        if (i % 4 == 0 && i != 0)
        {
            printf ("  %2.2X%2.2X%2.2X%2.2X\n",
                    static_cast < unsigned char > (packetData[i - 4]),
                    static_cast < unsigned char > (packetData[i - 3]),
                    static_cast < unsigned char > (packetData[i - 2]),
                    static_cast < unsigned char > (packetData[i - 1])
                   );
            c = 0;
        }
        b = static_cast < unsigned short int > (
                static_cast < unsigned char > (packetData[i]));
        char out[9];
        for (int j = 0; j < 8; j++)
        {
            if (b % 2)
                out[7 - j] = '1';
            else
                out[7 - j] = '0';
            c = 2 * c + b % 2;
            b = b >> 1;
        }
        out[8] = '\0';
        cerr << out;
        cerr << " ";
    }
}

void printInt(int x)
{
    char* packetData = reinterpret_cast < char* > (&x);
    for (int i = 0; i < 4; i++)
    {
        char out[9];
        unsigned short int b = static_cast < unsigned short int > (
                                   static_cast < unsigned char > (packetData[i]));
        for (int j = 0; j < 8; j++)
        {
            if (b % 2)
                out[7 - j] = '1';
            else
                out[7 - j] = '0';
            b = b >> 1;
        }
        out[8] = '\0';
        cerr << out;
        cerr << " ";
    }
    printf ("  %2.2X%2.2X%2.2X%2.2X\n",
            static_cast < unsigned char > (packetData[0]),
            static_cast < unsigned char > (packetData[1]),
            static_cast < unsigned char > (packetData[2]),
            static_cast < unsigned char > (packetData[3])
           );
    return ;
}


bool RtpSeqGreater (RtpSeqNumber a, RtpSeqNumber b)
{
    assert(a <= RTP_SEQ_MOD);
    assert(b <= RTP_SEQ_MOD);
//    assert(a >= 0);
//    assert(b >= 0);

    if ( (a > (RTP_SEQ_MOD - 20) && b < 20) || (a < 20 && b > RTP_SEQ_MOD - 20) )
    {
        //cpDebug(LOG_DEBUG_STACK,"backwards compare %d > %d", a, b);
        return (a < b);
    }
    else
        return (a > b);
}


bool RtpTimeGreater( RtpTime a, RtpTime b )
{
//    assert(a >= 0);
//    assert(b >= 0);
    assert(a <= RTP_TIME_MOD);
    assert(b <= RTP_TIME_MOD);

    if ( (a > (RTP_TIME_MOD - 1600) && b < 1600) || (a <1600 && b > RTP_TIME_MOD - 1600) )
    {
        //cpDebug(LOG_DEBUG_STACK,"backwards compare %d > %d", a, b);
        return (a < b);
    }
    else
        return (a > b);
}
