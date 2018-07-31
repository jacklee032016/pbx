/*
 * $Id: IsdnOpProceedingCall.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "IsdnOpBuilder.hxx"
#include "IeProgress.hxx"
#include "IeFacility.hxx"

using namespace Assist;

const Sptr <PbxState> IsdnOpProceedingCall::process( const Sptr <PbxEvent> event )
{
	if(checkCallMsg(event) != CALL_PROCEEDING )
	{
		return PBX_STATE_CONTINUE;
	}

	IsdnCtrlDevice  *ctrl = getIsdnCtrlDevice(event);
	Sptr <EndPoint> ep = event->getEndPoint();
	
	Sptr <MsgProceeding> proceeding = new MsgProceeding(ctrl );
	if(ctrl->getNtMode() )
	{
//		proceeding->setProgress(new IeProgress(0,IE_PROGRESS_LOCATION_RPIVATE_REMOTE_USER,8));
		proceeding->setFacility(new IeFacility );
	}
	
	Sptr <IsdnAudioChannel> bChan = NULL;
	bChan.dynamicCast(ep->getAudio());
	assert(bChan != 0);
	
	Sptr <IeChanId> ieChanId = new IeChanId(CHAN_ID_MODE_EXCLUSIVE, bChan->getChanId());
	proceeding->setChanId( ieChanId);
	proceeding->sendOut( ep);

	return lookupEPState(event,  ISDN_EP_STATE_IN_PROCEEDING);
}

