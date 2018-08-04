/*
 * $Id: PstnOpFarEndError.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "PstnOpBuilder.hxx"

using namespace Assist;

const Sptr <PbxState> PstnOpFarEndError::process( const Sptr <PbxEvent> event )
{
	if(checkCallMsg(event) != CALL_RELEASE )
	{
		return PBX_STATE_CONTINUE;
	}

#if 0
	Sptr <_CallRelease> rel;
	rel.dynamicCast(msg);
	if (rel == 0 )
	if( rel->getCause() == CALL_CAUSE_NO_ROUTE)
	{
		cpLog(LOG_DEBUG, "Release call beacuse of CALL_CAUSE_NO_ROUTE");
	}
	else if(rel->getCause() == CALL_CAUSE_BUSY_HERE)
	{
		cpLog(LOG_DEBUG, "Release call because of CALL_CAUSE_BUSY_HERE");
	}
#endif

	return lookupEPState(event,  PSTN_EP_STATE_ERROR);
}

