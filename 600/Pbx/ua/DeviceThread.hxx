#ifndef DeviceThread_H
#define DeviceThread_H

/*
 * $Id: DeviceThread.hxx,v 1.3 2007/03/16 19:33:56 lizhijie Exp $
 */

#include <Fifo.h>
#include <Sptr.hxx>
#include <ThreadIf.hxx>
#include <SipProxyEvent.hxx>

#include "ResGwDevice.hxx"

namespace Vocal
{

///
class DeviceThread : public ThreadIf
{
    public:
        ///
        DeviceThread( const Sptr < ResGwDevice > hwDevice );
        ///
        virtual ~DeviceThread();

    protected:
        ///
        virtual void thread();

        ///
        Sptr <ResGwDevice> device;
};
 
}

#endif
