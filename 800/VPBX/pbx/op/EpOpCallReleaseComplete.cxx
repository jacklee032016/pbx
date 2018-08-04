/*
 * $Id: EpOpCallReleaseComplete.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "cpLog.h"

#include "EpOpBuilder.hxx"
#include "PbxEvent.hxx"
#include "PbxState.hxx"

#include "PbxConfiguration.hxx"

using namespace Assist;

/* after send RELEASE or DISCONNECT to scheduler and wait reply(RELEASE_COMPLETE) from scheduler to free resources */
const Sptr <PbxState>  EpOpCallReleaseComplete::process( const Sptr <PbxEvent> event )
{
	cpLog( LOG_DEBUG, "EpOpStartTimer operation" );

	if (checkCallMsg(event) != CALL_RELEASE_COMPLETE )
	{
		return PBX_STATE_CONTINUE;
	}

	Sptr <EndPoint> ep = event->getEndPoint();
	assert(ep!= 0);

	Sptr <AudioChannel> audio = ep->getAudio();
	if(audio != 0)
	{/* this is reply for DISCONNECT */
		audio->deactivate();
		audio->setEndPoint( 0);
		ep->setAudio(0);
	}


	ep->reset();
	
	return lookupEPState(event, myNextState );/* for IDLE state except ISDN */
}

