/*
 * $Id: FeatureThread.hxx,v 1.2 2007/03/02 04:19:29 lizhijie Exp $
 */


#ifndef FeatureThread_H
#define FeatureThread_H

#include <Fifo.h>
#include <Sptr.hxx>
#include <ThreadIf.hxx>
#include <SipProxyEvent.hxx>

#include "SubscribeManager.hxx"

namespace Vocal
{

class FeatureThread : public ThreadIf
{
    public:
        ///
        FeatureThread( const Sptr < SubscribeManager > subManager );
        ///
        virtual ~FeatureThread();

    protected:
        ///
        virtual void thread();

        ///
        Sptr < SubscribeManager > subscribeManager;
};
 
}

#endif
