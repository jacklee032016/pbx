/*
 * $Id: GarbageCan.cxx,v 1.2 2007/03/01 20:04:32 lizhijie Exp $
 */


#include "global.h"
#include "GarbageCan.hxx"
#include "Garbage.hxx"


using Vocal::GarbageCan;
using Vocal::Garbage;
using Vocal::Threads::Mutex;
using std::list;


Mutex           GarbageCan::myMutex;
list<Garbage *> GarbageCan::myGarbage;


GarbageCan::GarbageCan()
{
}


GarbageCan::~GarbageCan()
{
    empty();
}
        

void 
GarbageCan::add(Garbage * garbage)
{
    myMutex.lock();
    myGarbage.push_back(garbage);
    myMutex.unlock();
}


void        
GarbageCan::empty()
{
    myMutex.lock();
    while ( myGarbage.size() != 0 )
    {
        delete myGarbage.back();
        myGarbage.pop_back();
    }
    myMutex.unlock();
}

