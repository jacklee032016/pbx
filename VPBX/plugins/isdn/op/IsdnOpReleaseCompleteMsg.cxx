/*
 * $Id: IsdnOpReleaseCompleteMsg.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "IsdnOpBuilder.hxx"

using namespace Assist;

const Sptr <PbxState> IsdnOpReleaseCompleteMsg::process( const Sptr <PbxEvent> event )
{
	if( checkIsdnMsgType(event) != ISDN_TYPE_RELEASE_COMPLETE_IND )
	{
		return PBX_STATE_CONTINUE;
	}

#if 0
	/* 1 : clear all info about call in EP */
	Sptr <IsdnEndPoint> isdnEp = NULL;
	isdnEp.dynamicCast(ep);
	assert( isdnEp != 0 );
	isdnEp->setCallId(0 );
	isdnEp->setFrameInfo( FRAME_INFO_NOT_EXIST);
#endif

	/*2 : return to IDLE state */
	return lookupEPState(event, ISDN_EP_STATE_WAIT_CLEAR_CR);
}

