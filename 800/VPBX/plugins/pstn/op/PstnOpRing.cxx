/*
 * $Id: PstnOpRing.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "PstnOpBuilder.hxx"
#include "PbxConfiguration.hxx"

using namespace Assist;

const Sptr <PbxState> PstnOpRing::process( const Sptr <PbxEvent> event )
{
	if( checkCallMsg(event) != CALL_SETUP )
	{
		return PBX_STATE_CONTINUE;
	}

	Sptr <CommonMsg> callMsg = isCallMsg( event);
	assert(callMsg != 0);
	
	Sptr <CallEvent> callEvent = isCallEvent(event);
	Sptr <EndPoint> ep = event->getEndPoint();
	assert(ep != NULL);

	ep->setPeerSignalType(callMsg->getPeerSignalType() );
	
	int nextState = PSTN_EP_STATE_RINGING;

	string peerNumber;
	if ( ep->checkExt() )
	{/* FXS device */
		/* get callerId for phone number display */
		peerNumber = callMsg->getFrom();
		if( PbxConfiguration::instance()->isCallIdOn() && PbxConfiguration::instance()->getPstnStandard() == PSTN_STANDARD_JAPAN )
		{
			nextState =PSTN_EP_STATE_CALLID;
			cpDebug(LOG_DEBUG, "Next state is PSTN_EP_STATE_CALLID");
		}
	}
	else		/* FXO device */
	{/* get phone number for external line */
		peerNumber = callMsg->getTo();
		string::size_type pos = peerNumber.find("#");
		cpDebug(LOG_DEBUG, "# is in poistion %d of %s" , pos, peerNumber.c_str() );
		if (pos != string::npos && pos< string::npos )
		{
			cpDebug(LOG_DEBUG, "# is in poistion %d of %s" , pos, peerNumber.c_str() );
			peerNumber= peerNumber.substr(pos+1, peerNumber.length()-pos-1);
		}	
	}

	ep->setPeerNumber(peerNumber.c_str());
	
	return lookupEPState(event, nextState);
}

