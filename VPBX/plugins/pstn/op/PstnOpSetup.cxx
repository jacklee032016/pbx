/*
 * $Id: PstnOpSetup.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "DeviceMgr.hxx"
#include "PstnOpBuilder.hxx"

using namespace Assist;

const Sptr <PbxState> PstnOpSetup::process( const Sptr <PbxEvent> event )
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

	setupMsg->setTo( ep->getDialing().c_str());

	if(ep->checkExt() )
	{/* for FXS, this is number it has */
		setupMsg->setFrom( ep->getPhoneNumber() );
		
		const char *accessCode = ep->getCtrlDevice()->getParent()->getAccessCode();
		int length = PbxConfiguration::instance()->getAccessLength();
		if(strncmp(accessCode, ep->getDialing().c_str(), length ) == 0)
		{/* this is a call from FXS to FXO, so using in-band signal: dtmf encode in audio */
			setupMsg->setSignalType( dtmf_signal_in_band);
		}
	}	
	else
	{/* for FXO, this is caller who originate this call */
		setupMsg->setFrom(ep->getPeerNumber().c_str());
		/* FXO always use in-band signal */
		setupMsg->setSignalType( dtmf_signal_in_band);
	}

	setupEvent->setCallMsg(setupMsg);
	setupEvent->setEndPoint(ep);
	setupEvent->setCallId( ep->getCallId() );

	sendSchedulerEvent(setupEvent);

	return lookupEPState(event, PSTN_EP_STATE_TRYING);
}

