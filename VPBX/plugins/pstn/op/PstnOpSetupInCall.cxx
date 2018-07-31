/*
 * $Id: PstnOpSetupInCall.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "PstnOpBuilder.hxx"
#include "PbxConfiguration.hxx"
#include "DigitEvent.hxx"

using namespace Assist;

/* process DTMF digit tx by peer (out-band signal) when in state of CALL */
const Sptr <PbxState> PstnOpSetupInCall::process( const Sptr <PbxEvent> event )
{
	if( checkCallMsg(event) != CALL_SETUP )
	{
		return PBX_STATE_CONTINUE;
	}

	Sptr <CommonMsg> callMsg = isCallMsg( event);
	assert(callMsg != 0);

	const char *num = callMsg->getTo();
	if(num==0)
		return PBX_STATE_DONE_WITH_EVENT;
	
	Sptr <EndPoint> pep = event->getEndPoint();
	assert(pep != NULL);

	if ( pep->checkExt() )
	{/* FXS device, ignore this DTMF digit  */
		
	}
	else		/* FXO device */
	{
		if( pep->getPeerSignalType() == dtmf_signal_out_band )
		{/* send by DSP engine */
			Sptr <DigitEvent> digit = new DigitEvent;
			digit->setEndPoint(pep);
			digit->myDigit = num[0]; 
			sendDeviceCommand(digit);
		}
	}

	return PBX_STATE_DONE_WITH_EVENT;
}

