/*
 * $Id: IsdnOpReleaseCompleteCall.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "IsdnOpBuilder.hxx"

using namespace Assist;

const Sptr <PbxState> IsdnOpReleaseCompleteCall::process( const Sptr <PbxEvent> event )
{
	if(checkCallMsg(event)!= CALL_RELEASE_COMPLETE )
	{
		return PBX_STATE_CONTINUE;
	}

	Sptr <EndPoint> ep = event->getEndPoint();

	IeCause *cause = new IeCause(CAUSE_LOCATION_LPN, CAUSE_T_NO_ROUTE_DESTINATION);
	Sptr <MsgReleaseComplete> relC = new MsgReleaseComplete( cause, getIsdnCtrlDevice(event) );
	relC->sendOut( ep);

	return lookupEPState(event, ISDN_EP_STATE_OUT_ALERTING);
}

