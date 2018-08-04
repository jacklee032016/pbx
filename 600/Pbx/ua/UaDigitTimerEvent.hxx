#ifndef UaDigitTimerEvent_H
#define UaDigitTimerEvent_H

/*
 * $Id: UaDigitTimerEvent.hxx,v 1.2 2007/03/02 04:19:29 lizhijie Exp $
 */

#include <Sptr.hxx>
#include <SipProxyEvent.hxx>
#include <Fifo.h>
#include <SipCallId.hxx>

#include "SipTransceiver.hxx"


namespace Vocal
{

typedef enum
{
    LoadGenStartCall,
    LoadGenEndCall,
    LoadGenRestartCall
} LoadGenSigType;


/** Inter-digit Timer Events
 *
 */
class UaDigitTimerEvent : public SipProxyEvent
{
    public:
        ///
        UaDigitTimerEvent( const Sptr < Fifo < Sptr < SipProxyEvent > > > outputFifo );
        ///
        virtual ~UaDigitTimerEvent();
        ///
        virtual const char* const name() const;

        ///
        void setSipStack( const Sptr < SipTransceiver > stack );
        ///
        const Sptr < SipTransceiver > getSipStack();

        ///
        void setLoadGenSignalType(LoadGenSigType signal);

        ///
        LoadGenSigType getLoadGenSigType();


        //TODO move callId to protected area
        ///
        Sptr < SipCallId > callId;

    private:
        ///
        Sptr < SipTransceiver > sipStack;
        ///
        LoadGenSigType lgSignalType;

};
 
}

#endif
