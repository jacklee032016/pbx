/*
* $Id: RandomHex.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/
#ifdef USE_SSL
#if defined(__linux__)
#include <openssl/rand.h>
#endif
#endif

#include "RandomHex.hxx"
#include "Lock.hxx"
#include "support.hxx"
#include "cpLog.h"

Assist::Threads::Mutex RandomHex::_mutex;

//#if !defined(__linux__)

#ifdef USE_SSL
void
RandomHex::initialize()
{
   assert(RAND_status() == 1);
}

Data
RandomHex::get(unsigned int len)
{
   unsigned char buffer[len];
   int ret = RAND_bytes(buffer, len);
   assert (ret == 1);
   
   Data result;
   result = convertToHex(buffer, len);
   
   return result;
}

#else

#include "CryptoRandom.hxx"

void
RandomHex::initialize()
{
}

Data
RandomHex::get(unsigned int len)
{
   Assist::Threads::Lock lock(_mutex);
   static CryptoRandom random;
   
   //unsigned char* buffer;
   unsigned char *buffer = new unsigned char[len];

   random.getRandom(buffer, len);
   
   Data result;
   result = convertToHex(buffer, len);
   delete[] buffer;
   return result;
}

#endif
