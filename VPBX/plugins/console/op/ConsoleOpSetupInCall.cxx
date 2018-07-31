/*
 * $Id: ConsoleOpSetupInCall.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */
 
#include "ConsoleOpBuilder.hxx"

#include "PbxAgent.hxx"
#include "PbxConfiguration.hxx"

using namespace Assist;

/*
* rx DTMF digit in SETUP call forward by peer out-band endpoint.
* send the number rxed to slave end point
*/
const Sptr <PbxState> ConsoleOpSetupInCall::process( const Sptr <PbxEvent> event )
{
	if(checkCallMsg(event) != CALL_SETUP  )
	{
		return PBX_STATE_CONTINUE;
	}
	Sptr <CommonMsg> commMsg = isCallMsg( event);
	if(commMsg == 0)
		return PBX_STATE_CONTINUE;

	Sptr <EndPoint> master =  getMasterEndPoint(event);
	const char *num = commMsg->getTo();
	if(num== 0 || strlen(num)== 0)
		return PBX_STATE_DONE_WITH_EVENT;

	for(size_t i=0; i< strlen(num); i++)
	{
		master->genDtmfEvent(num[i]);
	}

	return PBX_STATE_DONE_WITH_EVENT;
}

