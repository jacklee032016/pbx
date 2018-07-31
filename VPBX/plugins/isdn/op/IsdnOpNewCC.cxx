/*
 * $Id: IsdnOpNewCC.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "IsdnOpBuilder.hxx"

using namespace Assist;

const Sptr <PbxState> IsdnOpNewCC::process( const Sptr <PbxEvent> event )
{
	Sptr <IsdnMsg> isdnMsg = isIsdnMsg( event);
	if( isdnMsg == 0 )
		return PBX_STATE_CONTINUE;

	if ( isdnMsg->getType() != ISDN_TYPE_CC_NEW )
	{
//		cpLog(LOG_ERR, "ISDN MSG parse error, chcck '%s' carefully", isdnMsg->getName() );
		return PBX_STATE_CONTINUE;
	}

	cpLog(LOG_DEBUG_STACK, "'%s' is handled in %s", isdnMsg->getName(), name() );
	Sptr <EndPoint> ep = event->getEndPoint();
	assert(ep != 0);

	Sptr <IsdnEndPoint> isdnEp = NULL;
	isdnEp.dynamicCast(ep);
	assert(isdnEp != 0);

	ep->setCallId( new CallId() );
	
	if( isdnEp->getFrameInfo() != isdnMsg->getFrameInfo() )
	{
		cpLog(LOG_ERR, "This msg is not for this EndPoint");
//		Sptr <IeCause> cause = new IeCause(CAUSE_LOCATION_LPN, -ret );
//		Sptr <MsgReleaseComplete> relComMsg = new MsgReleaseComplete( cause);
		
		return lookupEPState(event, ISDN_EP_STATE_ERROR);
	}

	return PBX_STATE_DONE_WITH_EVENT;
}

