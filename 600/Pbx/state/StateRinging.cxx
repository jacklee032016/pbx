/*
 * $Id: StateRinging.cxx,v 1.2 2007/03/02 04:19:29 lizhijie Exp $
 */

#include "global.h"
#include "StateRinging.hxx"

#include "OpStartRinging.hxx"
#include "OpRingCancel.hxx"
#include "OpRingingInvite.hxx"
#include "OpAnswerCall.hxx"
#include "OpStopRinging.hxx"
#include "OpCwRing.hxx"
#include "OpQueryStatus.hxx"
#include "OpDeviceIdle.hxx"
#include "OpDeviceBusy.hxx"
#include "OpStartTimer.hxx"
#include "OpTimeOver.hxx"
#include "OpCancelTimer.hxx"
#include "OpAck.hxx"

using namespace Vocal;

StateRinging::StateRinging()
{
    //Modified by lijie	
    //addEntryOperator( new OpStartRinging );
    //added by lijie to query device status
    addEntryOperator( new OpStartTimer );
    addEntryOperator( new OpQueryStatus );
	
    //added by lijie to process device idle event
    addOperator( new OpDeviceIdle );
    //added by lijie to process device busy event
    addOperator( new OpDeviceBusy );

    addOperator( new OpRingCancel );
    addOperator( new OpRingingInvite );
    addOperator( new OpAnswerCall );
    addOperator( new OpCwRing );
    addOperator( new OpTimeOver );	

    /*added by wuaiwu */
    //addOperator( new OpAck );
    addExitOperator( new OpCancelTimer );	
    addExitOperator( new OpStopRinging );
}


StateRinging::~StateRinging()
{
}


const char* const
StateRinging::name() const
{
    return "StateRinging";
}
