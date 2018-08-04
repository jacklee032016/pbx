/*
* $Id: VoipOpRtpTimeoutInCall.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "VoipOpBuilder.hxx"

using namespace Assist;

/* process RTP timeout event when in state of IN_CALL */
const Sptr <PbxState> VoipOpRtpTimeoutInCall::process( const Sptr <PbxEvent> event )
{
	if( isTimeout(event) == false)
	{
		return PBX_STATE_CONTINUE;
	}
#if 0	
	Sptr <SipEvent> sipEvent;
	sipEvent.dynamicCast(event);
	assert(sipEvent!= 0);

	sipEvent->setErrorInfo(ERR_LINEDROP);
#endif
	cpLog(LOG_DEBUG, "RTP timeout in VOIP AudioChannel"	);
	sendCallSignal(event, CALL_DISCONNECT);

//	return lookupEPState(event, VOIP_EP_STATE_IDLE);
	return PBX_STATE_DONE_WITH_EVENT;
#if 0
    Sptr < UaHardwareEvent > signal;
    signal = new UaHardwareEvent( UaDevice::getDeviceQueue() );
    signal->type = HardwareSignalType;
    signal->setDeviceId(event->getDeviceId());
    signal->signalOrRequest.signal = DeviceSignalBusyStart;
    signal->setErrorInfo(ERR_LINEDROP);
    UaDevice::getDeviceQueue()->add( signal );

    Sptr < Fifo < Sptr < SipProxyEvent > > > eventQ = deviceEvent->getFifo();
    Sptr < UaDeviceEvent > sig= new UaDeviceEvent( eventQ );
   sig->type = DeviceEventHookDown;
   sig->setDeviceId( event->getDeviceId() );
   eventQ->add( sig );
    return 0;
#endif 
}

