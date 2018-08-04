/*
 * $Id: IsdnOpAlertingCall.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "IsdnOpBuilder.hxx"
#include "IeProgress.hxx"

using namespace Assist;

const Sptr <PbxState> IsdnOpAlertingCall::process( const Sptr <PbxEvent> event )
{
	if(checkCallMsg(event) != CALL_ALERTING )
	{
		return PBX_STATE_CONTINUE;
	}

	Sptr <EndPoint> ep = event->getEndPoint();

	IsdnCtrlDevice *ctrl = getIsdnCtrlDevice( event);

	Sptr <MsgAlerting> alerting = new MsgAlerting( ctrl );
	if(ctrl->getNtMode() )
		alerting->setProgress(new IeProgress(0,IE_PROGRESS_LOCATION_RPIVATE_REMOTE_USER,IE_PROGRESS_CONTENT_INBAND_SIGNAL));
	
	alerting->sendOut( ep);

//	if( ep->getState()->getStateType() == ISDN_EP_STATE_IN_SETUP )
//		return ep->findState( ISDN_EP_STATE_IN_ALERTING);

	return lookupEPState(event, ISDN_EP_STATE_IN_ALERTING );
}

