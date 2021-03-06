#ifndef NO_YARROW

#ifndef YARROWPSEUDORANDOM_HXX_
#define YARROWPSEUDORANDOM_HXX_
/*
* $Id: YarrowPseudoRandom.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "Data.hxx"
#include "yarrow.h"
#include "ystate.h"
#include "yexcep.h"
    
#define ENTROPY_BUF_SIZE 20 

class YarrowPseudoRandom
{
   public:
      int getRandom(unsigned char* buf, int num);
      ~YarrowPseudoRandom();
      static YarrowPseudoRandom* instance();
   private:
      YarrowPseudoRandom();
      static YarrowPseudoRandom* yarrowInst;
      static unsigned char entropyBuf[ENTROPY_BUF_SIZE];
      Yarrow_CTX yarrow;
};

#endif

#endif //NO_YARROW
