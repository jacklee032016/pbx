#ifndef RTPTHREAD_HXX_
#define RTPTHREAD_HXX_
/*
 * $Id: RtpThread.hxx,v 1.3 2007/03/03 18:39:28 lizhijie Exp $
 */

#include "Fifo.h"
#include "Sptr.hxx"
#include "ThreadIf.hxx"
#include "ResGwDevice.hxx"

namespace Vocal
{

///
class RtpThread : public ThreadIf
{
    public:
        ///
        RtpThread( const Sptr < ResGwDevice > hwDevice );
        ///
        virtual ~RtpThread();

    protected:
        ///
        virtual void thread();

        ///
        Sptr < ResGwDevice > device;
};
 
}

#endif

