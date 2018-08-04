#ifndef CRYPTORANDOM_HXX_
#define CRYPTORANDOM_HXX_
/*
 * $Id: CryptoRandom.hxx,v 1.2 2007/03/01 20:04:29 lizhijie Exp $
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
