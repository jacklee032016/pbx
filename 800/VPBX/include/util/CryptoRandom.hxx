#ifndef CRYPTORANDOM_HXX_
#define CRYPTORANDOM_HXX_
/*
* $Id: CryptoRandom.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include <sys/ioctl.h>

#ifdef __linux__
#include <asm/types.h>
#include <linux/random.h>
#endif


class CryptoRandom
{
    public:
        ///
        CryptoRandom();
        ///
        ~CryptoRandom();
        ///buf will contain the generated random. number, and num indicates the
        //number of bytes of random number required.
        int getCryptoRandom(unsigned char* buf, int num);
        ///
        int getRandom(unsigned char* buf, int num);

        ///
        int getPseudoRandom(unsigned char* buf, int num);
};

#endif
