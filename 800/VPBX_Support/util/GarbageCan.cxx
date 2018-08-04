/*
* $Id: GarbageCan.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include "GarbageCan.hxx"
#include "Garbage.hxx"


using Assist::GarbageCan;
using Assist::Garbage;
using Assist::Threads::Mutex;
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

