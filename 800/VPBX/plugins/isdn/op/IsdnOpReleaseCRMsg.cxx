/*
 * $Id: IsdnOpReleaseCRMsg.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "IsdnOpBuilder.hxx"

using namespace Assist;

const Sptr <PbxState> IsdnOpReleaseCRMsg::process( const Sptr <PbxEvent> event )
{
	if( checkIsdnMsgType(event) != ISDN_TYPE_RELEASE_CR )
	{
		return PBX_STATE_CONTINUE;
	}

	/*  : clear all info about call in EP */
	Sptr <EndPoint> ep = event->getEndPoint();
	assert( ep != 0 );

	ep->reset();

	Sptr <IsdnEndPoint> isdnEp = NULL;
	isdnEp.dynamicCast(ep);
	assert( isdnEp != 0 );
	isdnEp->setFrameInfo( FRAME_INFO_NOT_EXIST);

	/* : return to IDLE state */
	return lookupEPState(event, ISDN_EP_STATE_IDLE);
}

