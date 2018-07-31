/*
 * $Id: EpOpAddDigit.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "global.h"
#include "EpOpBuilder.hxx"
#include "EndPoint.hxx"
#include "PbxEvent.hxx"
#include "DigitEvent.hxx"
#include "DialingValidator.hxx"
#include "DigitTimeoutEvent.hxx"

#include "PbxAgent.hxx"

using namespace Assist;

const Sptr <PbxState> EpOpAddDigit::process( const Sptr <PbxEvent> event )
{
	Sptr <DigitEvent> digitEvent = isDigit(event);
	if ( digitEvent == 0 )
	{
		return PBX_STATE_CONTINUE;
	}

	if ( digitEvent->myDigit != ' ' )
	{
		cpLog( LOG_DEBUG, "Add Digit '%c'", digitEvent->myDigit );
		Sptr <EndPoint> ep = event->getEndPoint();
		assert( ep != 0 );
		ep->setDialing(digitEvent->myDigit);
		
		cancelTimer( event);

		Sptr <DialingValidator> dg = PbxAgent::instance()->getGateway()->getDialingValidator();
		assert (dg != 0);
		dialing_state_t dialingState = dg->addDigit( digitEvent );
#if 0
/* for debug*/
	if(strlen(ep->getDialing().c_str() )== 4)
		return PBX_STATE_CONTINUE;
#endif
		switch (dialingState)
		{
			case dialing_not_finished:
			{/* stop the process for this event : not continue the following operators in this state */
				setTimer(event, PbxConfiguration::instance()->getInterDigitTimeout() );
				return PBX_STATE_DONE_WITH_EVENT;
				break;
			}
			case dialing_finished:
			{/* continue to begin a call in the following PbxOperator, so Operator which begin a new call must be following this Operator */
				cpLog(LOG_DEBUG, "DIALING_FINISHED");
				return PBX_STATE_CONTINUE;
				break;
			}
			case dialing_error:
			{/* wrong dial and all others are represent by DigitTimeoutEvent */
				
				Sptr <DigitTimeoutEvent> timerEvent = new DigitTimeoutEvent();
				assert( timerEvent != 0 );
				timerEvent->setEndPoint(ep );
				sendGatewayEvent(timerEvent);
				
				return PBX_STATE_DONE_WITH_EVENT;
				break;
			}
			default:
				return PBX_STATE_DONE_WITH_EVENT;
				break;
		}	
		
	}

	return PBX_STATE_DONE_WITH_EVENT;
}

