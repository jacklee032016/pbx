#ifndef UaTimerEvent_H
#define UaTimerEvent_H

/*
 * $Id: UaTimerEvent.hxx,v 1.2 2007/03/02 04:19:29 lizhijie Exp $
 */

#include <Sptr.hxx>
#include <TimerEvent.hxx>
#include <Fifo.h>
#include <SipCallId.hxx>

#include "SipTransceiver.hxx"

namespace Vocal
{

/** Timeout event
 *
 */
class UaTimerEvent : public TimerEvent
{
    public:
        ///
        UaTimerEvent( const Sptr < Fifo < Sptr < SipProxyEvent > > > outputFifo );
        ///
        virtual ~UaTimerEvent();
        ///
        virtual const char* const name() const;

        ///
        void setCallId( const Sptr < SipCallId > callid );
        ///
        const Sptr < SipCallId > getCallId() const;

    protected:

    private:
        ///
        Sptr < SipCallId > callId;

};
 
}

#endif
