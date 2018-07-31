/*
 * $Id: IsdnOpDisconnectCall.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "IsdnOpBuilder.hxx"

using namespace Assist;

const Sptr <PbxState> IsdnOpDisconnectCall::process( const Sptr <PbxEvent> event )
{
	if(checkCallMsg(event) != CALL_DISCONNECT )
	{
		return PBX_STATE_CONTINUE;
	}

	Sptr <EndPoint> ep = event->getEndPoint();
	assert(ep!=0);
	IsdnCtrlDevice *ctrl = getIsdnCtrlDevice( event);

	cpLog(LOG_INFO," sending DISCONNECT");
	Sptr <IeCause> cause = new IeCause(CAUSE_LOCATION_USER, CAUSE_T_NORMAL_DISCONN);
	Sptr <MsgDisConnect> discMsg = new MsgDisConnect(cause, ctrl );

	if(ctrl->getNtMode() )
	{
		Sptr <IeCharge> charge = new IeCharge((unsigned char *)"9", IE_CHARGE_TOTAL_CHARGE);
		discMsg->setCharge( charge);
	}
	discMsg->sendOut( ep);

	if(ctrl->getNtMode() )
	{/* for NT, send DisConnect msg just enter into DisConnect Indication state. In this case ,the TA can not plsy busy tone
	* RELEASE msg can be used as call clear, so busy tone is played by TA. refer to specs vol. 3. p.158 
	*/
		cpLog(LOG_INFO," sending RELEASE ");
		Sptr <MsgRelease> relMsg = new MsgRelease(ctrl );
		relMsg->sendOut( ep);

		Sptr <MsgCCRelease> relCCMsg = new MsgCCRelease(ctrl );
		relCCMsg->sendOut( ep);
	}


	return lookupEPState(event, ISDN_EP_STATE_OUT_DISCONNECT );
}

