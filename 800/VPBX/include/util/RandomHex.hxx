#ifndef RandomHex_HXX_
#define RandomHex_HXX_
/*
* $Id: RandomHex.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "Mutex.hxx"
#include "Data.hxx"

class RandomHex
{
   public:
      static void initialize();
      static Data get(unsigned int len);

   private:
      static Assist::Threads::Mutex _mutex;
};

#endif

