/*
 * $Id: IsdnOpProceedingMsg.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "IsdnOpBuilder.hxx"

using namespace Assist;

const Sptr <PbxState> IsdnOpProceedingMsg::process( const Sptr <PbxEvent> event )
{
	if(checkIsdnMsgType(event) != ISDN_TYPE_PROCEEDING )
	{
		return PBX_STATE_CONTINUE;
	}

#if 0	
	/*only ChanId IE and Facility IE are provided in PROCEEDING msg */
	int bchId = proMsg->getChanId()->getChanId();

	if( ep->getAudio() != bchId	)
	{/* chan ID is not the audio channel we selected */
		Sptr <IeCause> cause = new IeCause(CAUSE_LOCATION_LPN, -ret );
		Sptr <MsgReleaseComplete> relComMsg = new MsgReleaseComplete( cause);
		pbxMsg->setIsdnMsg(relComMsg);
		return event->getEndPoint()->findState(ISDN_EP_STATE_ERROR);
	}
#endif

#if 0
	/* when rx releaseComplete msg, then we are in the last step of EP clear 
	* Call clear has been finished now
	*/
	sendCallSignal(event, CALL_PROCEEDING);
#endif	

	return lookupEPState(event, ISDN_EP_STATE_OUT_PROCEEDING);
}

