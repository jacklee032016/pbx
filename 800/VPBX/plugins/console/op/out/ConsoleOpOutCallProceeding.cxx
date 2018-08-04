/*
 * $Id: ConsoleOpOutCallProceeding.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "ConsoleOpBuilder.hxx"

using namespace Assist;

const Sptr <PbxState> ConsoleOpOutCallProceeding::process( const Sptr <PbxEvent> event )
{
	if(checkCallMsg(event) != CALL_PROCEEDING )
	{
		return PBX_STATE_CONTINUE;
	}

	return PBX_STATE_DONE_WITH_EVENT;
}

