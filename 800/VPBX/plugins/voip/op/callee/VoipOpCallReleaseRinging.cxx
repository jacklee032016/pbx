/*
* $Id: VoipOpCallReleaseRinging.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "VoipOpBuilder.hxx"

using namespace Assist;

/* process the _CallRelease return by scheduler in state of RINGING */
const Sptr <PbxState> VoipOpCallReleaseRinging::process( const Sptr <PbxEvent> event )
{
	if(checkCallMsg(event) != CALL_RELEASE )
	{
		return PBX_STATE_CONTINUE;
	}

	Sptr <SipContext> context = getSipContext(event);
	assert( context != 0 );

	Sptr <InviteMsg> msg = context->getRingInvite();
	assert( msg != 0 );
	Sptr <VoipEndPoint> vep = getVoipEndPoint(event);

	/* added cause check in _CallRelease */
	StatusMsg status( *msg, SIP_STATUS_CODE_CERROR_BUSY_HERE );
	
	vep->getSipStack()->sendReply( status );

//removed by lijie 2005-06-03   
#if 0	
      calls->deleteCall( *(UaDevice::instance()->getCallId( event->getDeviceId() )) ,event->getDeviceId());
    // Clear call id in device
    UaDevice::instance()->setCallId( 0 ,event->getDeviceId() );
#endif

	context->setCalleeIdle(false);

	vep->reset();
	
	return lookupEPState(event, VOIP_EP_STATE_IDLE );
}

