/*
* $Id: Random.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include "Random.hxx"
#include "MACAddress.hxx"
#include <cstdlib>
#include <unistd.h>
#include <time.h>
#include <sys/times.h>



using Assist::Random;
using Assist::Transport::MACAddress;


bool            Random::myInit = false;
unsigned int    Random::mySeed = 0;


Random::Random()
{
    init();
}    
        

Random::~Random()
{
}


int     
Random::operator()()
{
    return ( get() );
}
        

int        
Random::get()
{
    return ( rand() );
}


void            
Random::init(unsigned int seed)
{
    if ( myInit == true )
    {
        return;
    }
    
    MACAddress	    macAddr;
    #ifndef WIN32
    tms     	    emptyTms;
    #endif

    if ( seed == 0 )
    {
        mySeed  = 	macAddr.high() 
                ^   macAddr.low() 
                ^   static_cast<unsigned>(getpid())
                #ifndef WIN32
                ^   static_cast<unsigned>(times(&emptyTms))
                #endif
                ^   time(0);
    }
    else
    {
        mySeed = seed;
    }

    srand(mySeed);

    myInit = true;
};


unsigned int
Random::seed()
{
    return ( mySeed );
}
