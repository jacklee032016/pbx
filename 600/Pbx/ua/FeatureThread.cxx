/*
 * $Id: FeatureThread.cxx,v 1.2 2007/03/02 04:19:29 lizhijie Exp $
 */

#include "global.h"
#include <cassert>

#include "FeatureThread.hxx"

using namespace Vocal;

///
FeatureThread::FeatureThread( const Sptr < SubscribeManager > subManager )
{
    subscribeManager = subManager;
}

///
FeatureThread::~FeatureThread()
{}


///

void
FeatureThread::thread()
{
    while ( true )
    {
        subscribeManager->subscribeMain();
    }
}

