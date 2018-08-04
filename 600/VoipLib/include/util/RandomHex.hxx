#ifndef RandomHex_HXX_
#define RandomHex_HXX_
/*
 * $Id: RandomHex.hxx,v 1.2 2007/03/01 20:04:29 lizhijie Exp $
 */


#include "Mutex.hxx"
#include "Data.hxx"

class RandomHex
{
   public:
      static void initialize();
      static Data get(unsigned int len);

   private:
      static Vocal::Threads::Mutex _mutex;
};

#endif

