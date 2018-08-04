/*
 * $Id: VoipOperator.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */
#include "SipContext.hxx"

#include "PbxEvent.hxx"
#include "CallEvent.hxx"
#include "CallId.hxx"
#include "DigitTimeoutEvent.hxx"
#include "EpTimerEvent.hxx"
#include "CallMsgBuilder.hxx"
#include "VoipOperator.hxx"

using namespace Assist;

//Sptr < Fifo < Sptr <PbxEvent> > >VoipOperator::myBackFifo = 0;

VoipOperator::VoipOperator(const string& name)
	:EpOperator(name)
{
}

VoipOperator::~VoipOperator()
{}

Sptr <SipEvent> VoipOperator::isSipEvent( const Sptr <PbxEvent> event )
{
	assert(event!=0);
	doCpLog(event);

	Sptr <SipEvent> sipEvent;
	sipEvent.dynamicCast( event );
	if ( sipEvent == 0 )
	{
		return 0;
	}

	return sipEvent;
}

Sptr <SipMsg> VoipOperator::isSipMsg( const Sptr <PbxEvent> event )
{
	Sptr <SipEvent> sipEvent = isSipEvent(event);

	if ( sipEvent == 0 )
	{
		return 0;
	}

	Sptr <SipMsg> sipMsg = sipEvent->getSipMsg();
	assert( sipMsg != 0 );

	return sipMsg;
}

Sptr <StatusMsg> VoipOperator::isStatusMsg( const Sptr <PbxEvent> event )
{
	Sptr <StatusMsg> statusMsg;
	statusMsg.dynamicCast( isSipMsg(event));

	return statusMsg;
}

Method  VoipOperator::getSipMsgType(Sptr <PbxEvent> event)
{
	Sptr <SipMsg>  sipMsg = isSipMsg(event);

	if (sipMsg == 0 )
	{
		return SIP_INVALIDMSG;
	}
	return sipMsg->getType();
}

Sptr <VoipEndPoint> VoipOperator::getVoipEndPoint(Sptr <PbxEvent> event)
{
	Sptr <VoipEndPoint> vep;
	vep.dynamicCast(event->getEndPoint() );
	assert(vep!= 0);

	return vep;
}

Sptr <SipContext> VoipOperator::getSipContext(Sptr <PbxEvent> event)
{
	return getVoipEndPoint(event)->getSipContext();
}

