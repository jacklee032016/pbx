/*
 * $Id: OpRingCancel.cxx,v 1.3 2007/03/07 18:24:08 lizhijie Exp $
 */

#include "global.h"
#include <SipEvent.hxx>
#include <CancelMsg.hxx>
#include <ByeMsg.hxx>

#include "OpRingCancel.hxx"
#include "UaStateMachine.hxx"
#include "UaDevice.hxx"
#include "UaCallContainer.hxx"
#include "UaCallInfo.hxx"
#include "UaConfiguration.hxx"
#include "UaHardwareEvent.hxx"

#if __CDR_OPEN__
#include "SimulateDB.hxx"
using namespace SDB;
#endif  
using namespace Vocal;


OpRingCancel::OpRingCancel()
{}


OpRingCancel::~OpRingCancel()
{}


const char* const OpRingCancel::name() const
{
	return "OpRingCancel";
}


const Sptr <State> OpRingCancel::process( const Sptr <SipProxyEvent> event )
{
	cpDebug(LOG_DEBUG, "OpRingCancel operation" );

	Sptr < SipEvent > sipEvent;
	sipEvent.dynamicCast( event );
	if ( sipEvent == 0 )
	{
		return 0;
	}
	Sptr < SipMsg > sipMsg = sipEvent->getSipMsg();
	assert( sipMsg != 0 );

	// either a cancel or a bye message will cancel the ringing
	// and terminate the call
	Sptr < StatusMsg > status;

	Sptr < CancelMsg > cancelMsg;
	cancelMsg.dynamicCast( sipMsg );

	Sptr < ByeMsg > byeMsg;
	byeMsg.dynamicCast( sipMsg );

	if (!cancelMsg  && !byeMsg ) return 0;

	Sptr < UaCallInfo > call;
	call.dynamicCast( event->getCallInfo() );
	assert( call != 0 );


#if 0
	if ( cancelMsg != 0 )
	{
	status = new StatusMsg(*cancelMsg, 200/*OK*/);
	contact = call->findContact( *cancelMsg );
	}
	else if ( byeMsg != 0 )
	{
	status = new StatusMsg(*byeMsg, 200/*OK*/);
	contact = call->findContact( *byeMsg );
	}
	else
	{
	return 0;
	}
	sipEvent->getSipStack()->sendReply( *status );


	Sptr < InviteMsg > msg = call->getRingInvite();
	assert( msg != 0 );
	Sptr < StatusMsg > inviteStatus = new StatusMsg(*msg, 487/*Request Terminated*/);
	sipEvent->getSipStack()->sendReply( *inviteStatus );
#endif 

	int statusno = 200;

	Sptr < Contact > contact = call->findContact( *sipMsg );;
	if ( contact == 0 )
	{
		cpDebug(LOG_DEBUG, "No known contact for this messagemake" );
		statusno = 481;
	}

	Sptr < Contact > curContact = call->getContact();
	assert( curContact != 0 );
	if ( *curContact != *contact )
	{
		// CANCEL not for current contact
		cpDebug(LOG_DEBUG, "CANCEL not for current contact" );
		statusno = 481;
	}

	if(cancelMsg !=0)
	{
		cpDebug(LOG_DEBUG,"got CANCEL");
		status = new StatusMsg(*cancelMsg, statusno);
	}
	else
	{
		cpDebug(LOG_DEBUG,"got BYE");
		status = new StatusMsg(*byeMsg, statusno);
	}
	sipEvent->getSipStack()->sendReply( *status );

	if (statusno == 481)
	{
		return 0;
	}

	Sptr < InviteMsg > msg = call->getRingInvite();
	assert( msg != 0 );
	Sptr < StatusMsg > inviteStatus = new StatusMsg(*msg, 487/*Request Terminated*/);
	sipEvent->getSipStack()->sendReply( *inviteStatus );

	cpDebug(LOG_DEBUG, "Cancel Ringing" );

#if 0  //removed by lijie 2005-06-03
	// Remove call info from call container
	Sptr < UaCallContainer > calls;
	calls.dynamicCast( event->getCallContainer() );
	assert( calls != 0 );

	//calls->deleteCall( *(UaDevice::instance()->getCallId()) );
	SipCallId callId = call->getContact()->getInviteMsg().getCallId();

	calls->deleteCall( callId, event->getDeviceId() );
#endif

	Sptr < UaHardwareEvent > signal = new UaHardwareEvent( UaDevice::getDeviceQueue() );
	//Stop sending ringback tone if necessary
	if (UaConfiguration::instance()->getProvideRingback() && call->getContact()->getStatus() == 183 )
	{
		cpDebug(LOG_DEBUG, "Stop sending ringback tone");
		signal->type = HardwareAudioType;
		signal->signalOrRequest.request.type = AudioStop;

		signal->setDeviceId( event->getDeviceId() );

		UaDevice::getDeviceQueue()->add( signal );
	}
	else
	{
	// release the rtp port if one was reserve
	//UaDevice::instance()->releaseRtpPort( event->getDeviceId() );
	}

	//cout << "Ready" << endl;

#if __CDR_OPEN__
	/* added by wuaiwu 2005/5/11 */
	if (SimulateDB::instance() != NULL)
	{
	int deviceID = event->getDeviceId();
	SimulateDB::instance()->addCdrClient(sipMsg, deviceID, CALL_END);
	}

	/* end of added */
#endif
	Sptr < UaStateMachine > stateMachine;
	stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
	assert( stateMachine != 0 );


	return stateMachine->findState( "StateIdle" );
}

