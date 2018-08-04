/*
 * $Id: IsdnOpAlertingMsg.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "IsdnOpBuilder.hxx"

using namespace Assist;

const Sptr <PbxState> IsdnOpAlertingMsg::process( const Sptr <PbxEvent> event )
{
	if(checkIsdnMsgType(event) != ISDN_TYPE_ALERTING )
	{
		return PBX_STATE_CONTINUE;
	}

	/* only Progress IE is provided in ALERTING msg 
	* In this IE, location=4(RPIVATE_REMOTE_USER) , progress=8(INBAND_SIGNAL)
	*/
#if 0	
	Sptr <IeProgress> progress = alertMsg->getProgress();
//	progress
#endif

	sendCallSignal(event, CALL_ALERTING);

	return lookupEPState(event, ISDN_EP_STATE_OUT_ALERTING);
}

