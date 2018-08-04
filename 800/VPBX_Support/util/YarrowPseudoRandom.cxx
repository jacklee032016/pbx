#ifndef NO_YARROW

#include "global.h"
#include "CryptoRandom.hxx"
#include "YarrowPseudoRandom.hxx"
#include "cpLog.h"

extern "C" {
   int yarrow_verbose = 0;
};

unsigned char YarrowPseudoRandom::entropyBuf[ENTROPY_BUF_SIZE];
YarrowPseudoRandom* YarrowPseudoRandom::yarrowInst = 0; 

YarrowPseudoRandom::YarrowPseudoRandom()
{
   //initialize yarrow
   //yarrow_verbose =1 ;
    int ret = Yarrow_Init(&yarrow, NULL);
    if (ret != YARROW_OK && ret != YARROW_NOT_SEEDED)
    {
        assert(0);
    }
    else
    {
       unsigned r1, r2;
       Yarrow_New_Source( &yarrow, &r1 );
       Yarrow_New_Source( &yarrow, &r2 );

       for (int i=0; i<7; i++)
       {
          CryptoRandom crypto;
          crypto.getCryptoRandom(entropyBuf, 20);
          Yarrow_Input(&yarrow, r1, entropyBuf, 20, 3*20);
          //cout << Yarrow_Str_Error( ret ) << endl;
       }

       for (int i=0; i<40; i++)
       {
          CryptoRandom crypto;
          crypto.getCryptoRandom(entropyBuf, 4);
          Yarrow_Input(&yarrow, r2, entropyBuf, 20, 2*4);
          //cout << Yarrow_Str_Error( ret ) << endl;
       }
    }
}
    
    
YarrowPseudoRandom::~YarrowPseudoRandom()
{
}
    
    
YarrowPseudoRandom* YarrowPseudoRandom::instance()
{
    if (yarrowInst == 0)
    {
	yarrowInst = new YarrowPseudoRandom();
    }
    return yarrowInst;
}
    

int YarrowPseudoRandom::getRandom(unsigned char* buf, int num)
{
   int ret = Yarrow_Output(&yarrow, buf, num);
   assert(ret == YARROW_OK);
   return ret;
}

#endif
