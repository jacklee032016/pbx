/*
 * $Id: IsdnOpReleaseCall.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "IsdnOpBuilder.hxx"


using namespace Assist;

const Sptr <PbxState> IsdnOpReleaseCall::process( const Sptr <PbxEvent> event )
{
	if(checkCallMsg(event) != CALL_RELEASE )
	{
		return PBX_STATE_CONTINUE;
	}


	IsdnCtrlDevice *ctrl = getIsdnCtrlDevice(event);
	Sptr <EndPoint> ep = event->getEndPoint();
	int state = ep->getState()->getStateType();
	/* 1 : send release to peer */

//	if ( ep->getState()->getStateType()>= ISDN_EP_STATE_IN_ALERTING  && ep->getState()->getStateType() <=ISDN_EP_STATE_OUT_ALERTING)
	if ( state > ISDN_EP_STATE_IDLE  && state < ISDN_EP_STATE_CONNECT )
	{

		Sptr <IeCause> iec = new IeCause(CAUSE_LOCATION_USER,CAUSE_T_NORMAL_DISCONN);
		assert(iec != 0);		

		/* call originated from ISDN and released by by peer such as VOIP */
		cpLog(LOG_INFO,"sending peer DISCONNECT");
		Sptr <MsgDisConnect> disc = new MsgDisConnect(iec, ctrl );
		disc->sendOut( ep);

		if (state == ISDN_EP_STATE_IN_SETUP ||
			state == ISDN_EP_STATE_IN_PROCEEDING ||
			state == ISDN_EP_STATE_IN_ALERTING )
		{
			cpLog(LOG_INFO," sending RELEASE ");
			Sptr <MsgRelease> relMsg = new MsgRelease( iec, ctrl );
			relMsg->sendOut( ep);
		}
	}		

	if( ep->getState()->getStateType() == ISDN_EP_STATE_IN_SETUP)
	{
		Sptr <MsgReleaseComplete> relComMsg = new MsgReleaseComplete(ctrl );
		relComMsg->sendOut( ep );
	}
	else
	{
		Sptr <IeCause> ieCause = new IeCause(CAUSE_LOCATION_USER,CAUSE_T_NORMAL_DISCONN);
		assert(ieCause != 0);

		cpLog(LOG_INFO," sending RELEASE ");
		Sptr <MsgRelease> relMsg = new MsgRelease( ieCause,ctrl );
		relMsg->sendOut( ep);

		Sptr <MsgCCRelease> relCCMsg = new MsgCCRelease( ctrl );
		relCCMsg->sendOut( ep);

	}
	
//	if( ep->getState()->getStateType() == ISDN_EP_STATE_IN_SETUP )
//		return ep->findState( ISDN_EP_STATE_IN_ALERTING);
	/* 2 : enter into DISCONNECT_OUT for release_complete msg */
	return lookupEPState(event, ISDN_EP_STATE_OUT_DISCONNECT );
}

