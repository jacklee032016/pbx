/*
* $Id: VoipOpMsgInviteAuthorIdle.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "VoipOpBuilder.hxx"

#include "Verify.hxx"

using Assist::Crypto::Md5;

using namespace Assist;

/* process Timeout event for authentication when in state of IDLE */
const Sptr <PbxState> VoipOpMsgInviteAuthorIdle::process( const Sptr <PbxEvent> event )
{
	if( isTimeout(event) == false)
		return PBX_STATE_CONTINUE;

	/* remove SipContext for this endpoint */
	Sptr <EndPoint> ep = event->getEndPoint();
	assert(ep != 0);
	ep->reset();
	
	return PBX_STATE_CONTINUE;
}

