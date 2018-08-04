/*
* $Id: CryptoRandom.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include <cerrno>
#include <cstdlib>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "CryptoRandom.hxx"
#include "cpLog.h"
#include "global.h"
#include "support.hxx"


static bool srandomCalled = false;

CryptoRandom::CryptoRandom()
{
}

CryptoRandom::~CryptoRandom()
{
}

// Note that this call will block if there is not enough entropy gathered by the
// kernel. It will eventually return when enough data has been gathered. Use
// getPseudoRandom if delay may be a problem. 
int CryptoRandom::getCryptoRandom(unsigned char* buf, int num)
{
#ifdef USE_CRYPTO_RANDOM
    ssize_t len = 0;

    //open the io device /dev/random.
    // if no blocking is desired, use /dev/urandom instead. 
    FILE* fp = fopen( "/dev/urandom", "r");
    if ( fp == 0 )
    {
       cpLog(LOG_INFO, "CryptoRandom::unable to open /dev/random. Use pseudo random instead.");
       return getPseudoRandom(buf, num);
    }
    else
    {
       // by reading it this way, we will not do a short read. 
       if ( (len = fread(buf, 1, num, fp)) != num)
       {
          cpLog(LOG_INFO, "CryptoRandom::Error reading from /dev/random (%d/%d) ferror=%d. Use pseudo random instead.", len, num, ferror(fp));
          fclose(fp);
          return getPseudoRandom(buf, num);
       }
    }
    //close the io device /dev/random.
    fclose(fp);

    return len;
#else
    assert(0);
    return 0;
#endif
}


#ifndef WIN32
// this is still broken
int CryptoRandom::getPseudoRandom(unsigned char* buf, int num)
{
   int count=0;
   for (count=0; count < (num-3); count+=4)
   {
      //generate a random number;
      if (!srandomCalled)
      {
         struct timeval tv;
         gettimeofday(&tv, 0);
         
         srandom((unsigned int)(tv.tv_usec ^ tv.tv_sec));
         srandomCalled = true;
      }
      
      int rand = random();
      memcpy(buf+count, &rand, 4);  //copy 4 bytes from rand to buf.
   }
   
   // now copy the remainder
   int remainder = num % 4;
   if (remainder)
   {
      int rand = random();
      memcpy(buf+count, &rand, remainder);  //copy 4 bytes from rand to buf.
   }
   
   return num;
}
#else
int CryptoRandom::getPseudoRandom(unsigned char* buf, int num)
{
    //generate a random number;
    struct timeval tv;
    gettimeofday(&tv, 0);

    if (!srandomCalled)
    {
        srand((unsigned int)(tv.tv_usec ^ tv.tv_sec));
        srandomCalled = true;
    }
    int r = rand();
    r += tv.tv_usec; ;

    memcpy(buf, &r, 4);  //copy 4 bytes from rand to buf.
    return num;
}
#endif // WIN32

//char* CryptoRandom::getRandom()
int CryptoRandom::getRandom(unsigned char* buf, int num)
{
    int len;

#  ifdef USE_CRYPTO_RANDOM
    len = getCryptoRandom(buf, num);
#  else
    len = getPseudoRandom(buf, num);
#  endif

    assert (len == num);
    return len;
}
