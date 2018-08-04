#ifndef TIMEFUNC_HXX_
#define TIMEFUNC_HXX_
/*
* $Id: TimeFunc.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include <sys/time.h>
#include <unistd.h>
#include <iostream>

class FunctionTimer
{
   public:
      FunctionTimer() {start(); end(); }

      void start() {gettimeofday(&s1, 0); }
      void end() { gettimeofday(&s2, 0); }

   private:
      struct timeval s1, s2;
      friend inline ostream& operator<<(ostream& s, const FunctionTimer& f);
      
};

inline ostream& operator<<(ostream& s, const FunctionTimer& f) 
{
   unsigned long long c;
   c = ( ( (f.s2.tv_sec - f.s1.tv_sec) * 1000) +
         ( (f.s2.tv_usec - f.s1.tv_usec) / 1000) );
   s << c << "ms";
   return s;
}

#endif
