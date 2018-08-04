#ifndef UaDeviceEvent_H
#define UaDeviceEvent_H

/*
 * $Id: UaDeviceEvent.hxx,v 1.2 2007/03/02 04:19:29 lizhijie Exp $
 */

#include "Sptr.hxx"
#include "SipProxyEvent.hxx"
#include "Fifo.h"
#include "SipCallId.hxx"
#include "DeviceEvent.hxx"

#include "SipTransceiver.hxx"

namespace Vocal
{

//TODO move typedef's into UaDeviceEvent
///
typedef enum
{
    DeviceSignalNULL,
    DeviceSignalDialToneStart,
    DeviceSignalDialToneStop,
    DeviceSignalRingStart,
    DeviceSignalRingStop,
    DeviceSignalRemoteRingbackStart,
    DeviceSignalRemoteRingbackStop,
    DeviceSignalLocalRingbackStart,		/* 7 */
    DeviceSignalLocalRingbackStop,
    DeviceSignalBusyStart,
    DeviceSignalBusyStop,
    DeviceSignalFastBusyStart,
    DeviceSignalFastBusyStop,
    DeviceSignalErrorToneStart,
    DeviceSignalErrorToneStop,
  //added by lijie to query device status
    DeviceSignalQueryStatus,
    DeviceSignalDigit0,
    DeviceSignalDigit1,
    DeviceSignalDigit2,
    DeviceSignalDigit3,
    DeviceSignalDigit4,
    DeviceSignalDigit5,
    DeviceSignalDigit6,
    DeviceSignalDigit7,
    DeviceSignalDigit8,
    DeviceSignalDigit9,
    DeviceSignalDigitStar,
    DeviceSignalDigitHash,
    DeviceSignalDigitA,
    DeviceSignalDigitB,
    DeviceSignalDigitC,
    DeviceSignalDigitD,
    DeviceSignalFwding,
    DeviceSignalStartCallWaitingBeep,
    DeviceSignalStopCallWaitingBeep,
    // added by wuaiwu, 2005/10/11
    DeviceSignalDialErrorTone,
    DeviceSignalConnErrorTone,
    DeviceSignalNetDropTone
} DeviceSignalType;



/** Events from a device
 *
 */
class UaDeviceEvent : public SipProxyEvent
{
    public:
        ///
        UaDeviceEvent( const Sptr < Fifo < Sptr < SipProxyEvent > > > outputFifo );
        ///
        virtual ~UaDeviceEvent();
        ///
        virtual const char* const name() const;

        ///
        void setSipStack( const Sptr < SipTransceiver > stack );
        ///
        const Sptr < SipTransceiver > getSipStack();

        //TODO move type to protected area
        ///
        DeviceEventType type;
        ///
        Sptr < SipCallId > callId;
	///
	Data text;

    protected:

    private:
        ///
        Sptr < SipTransceiver > sipStack;
};
 
}

#endif
