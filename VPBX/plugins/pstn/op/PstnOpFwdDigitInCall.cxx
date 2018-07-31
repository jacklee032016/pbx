/*
 * $Id: PstnOpFwdDigitInCall.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "PstnOpBuilder.hxx"

using namespace Assist;

/* FXS forward DTMF digit rx from driver to peer when in the state of CALL */
const Sptr <PbxState> PstnOpFwdDigitInCall::process( const Sptr <PbxEvent> event )
{
	Sptr <DigitEvent> digitEvent = isDigit(event);
	if ( digitEvent == 0 )
	{
		cpLog(LOG_DEBUG, "This is not a DigitEvent");
		return PBX_STATE_CONTINUE;
	}

	Sptr <CallEvent> setupEvent = new CallEvent(event->getFifo());
	Sptr <CommonMsg> setupMsg = new _CallSetup;
	Sptr <EndPoint> ep = event->getEndPoint();
	assert(ep!=0);

	char num[10];
	num[0] = digitEvent->myDigit;
	num[1] = 0;
	setupMsg->setTo( num);

	setupEvent->setCallMsg(setupMsg);
	setupEvent->setEndPoint(ep);
	setupEvent->setCallId( ep->getCallId());

	sendSchedulerEvent(setupEvent);

	return PBX_STATE_DONE_WITH_EVENT;
}

