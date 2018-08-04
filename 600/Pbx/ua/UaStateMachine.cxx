/*
 * $Id: UaStateMachine.cxx,v 1.2 2007/03/02 04:19:29 lizhijie Exp $
 */

#include "global.h"
#include "State.hxx"
#include "UaStateMachine.hxx"
#include "UaCallInfo.hxx"

#include "StateBusy.hxx"
#include "StateCallEnded.hxx"
#include "StateDialing.hxx"
#include "StateError.hxx"
#include "StateFastBusy.hxx"
#include "StateIdle.hxx"
#include "StateInCall.hxx"
#include "StateRinging.hxx"
#include "StateTrying.hxx"
#include "StateInCallDialing.hxx"
#include "StateReRinging.hxx"
#include "StateInCallTrying.hxx"
#include "StateIn2Calls.hxx"
#include "StateBeingXferred.hxx"
#include "StateBlindXferring.hxx"
#include "StateConsultXferring.hxx"
#include "StateInCallBusy.hxx"
#include "StateInCallFastBusy.hxx"
#include "StateConferencing.hxx"
#include "StateBusyHere.hxx"
#include "StateOnhold.hxx"

#include "StatePbxDialing.hxx"
#include "StatePbxRinging.hxx"
#if 0
#include "StatePbxInCall.hxx"
#endif

#if 0
#include "StateAutoIdle.hxx"
#include "StateAutoTrying.hxx"
#include "StateAutoInCall.hxx"
#include "StateAutoRS.hxx"
#endif

#include "UaConfiguration.hxx"
#include "UaDevice.hxx"

using namespace Vocal;
typedef struct
{
	const char *name;
	const char *describe;
}_DESCRIBE_;

#if 0
static _DESCRIBE_	des[] =
{
    	{"StateBeingXferred", "being transferred"},
	{"StateBlindXferring", "is blind transferring"},
	{"StateBusy", "is busy now"},
	{"StateBusyHere", "is busy here"},
	{"StateCallEnded", "is ending call"},
	{"StateConferencing", "is on conferencing"},
	{"StateConsultXferring", "is consult transferring"},
	{"StateDialing", "is dialing"},
	{"StateError", "is error"},
	{"StateFastBusy", "is busy"},
	{"StateIdle", "is idle"},
	{"StateIn2Calls", "is in two calls"},
	{"StateInCall", "is in call"},
	{"StateInCallBusy", "is busy in call"},
	{"StateInCallDialing", "is dialing in call"},
	{"StateInCallFastBusy", "is in call fast busy"},
	{"StateInCallTrying", "is trying in call"},
    	{"StateOnhold", "is on hold"},
	{"StatePbxDialing", "is being dialed"},
	{"StatePbxRinging", "is ringing"},
	{"StateReRinging", "is reringing"},
	{"StateRinging", "is ringing"},
	{"StateTrying", "is trying"},
	{NULL, NULL}
};
#endif 

static _DESCRIBE_	des[] =
{
	{"StateIdle", "is idle"},
	{"StateDialing", "is dialing"},
	{"StateInCall", "is in call"},
	{"StateError", "is error"},
	{"StateFastBusy", "is busy"},
	{"Unknow", "Unknow"}
};

///
UaStateMachine::UaStateMachine()
{
	cpDebug(LOG_DEBUG, "User Agent State Machine" );

#if 0
    if ( UaConfiguration::instance()->getLoadGenOn() )
    {
        if (UaConfiguration::instance()->getRunMode() == "RSTest")
        {
            cpDebug(LOG_DEBUG, "*** StateMachine for RS Test ***");
            addState( new StateAutoRS );
        }
        else if (UaConfiguration::instance()->getRunMode() == "Calling")
        {
            // State for calling
            cpDebug(LOG_DEBUG, "*** StateMachine for Calling Load Gen Used ***");
            addState( new StateAutoIdle );
            addState( new StateAutoTrying );
            addState( new StateAutoInCall );
        }
        else if (UaConfiguration::instance()->getRunMode() == "Receiving")
        {
            // State for receiving
            cpDebug(LOG_DEBUG, "*** StateMachine for Receiving Load Gen Used ***");
            addState( new StateAutoIdle );
            addState( new StateAutoInCall );
        }
        else
        {
            cpLog(LOG_ERR, "******* RUNNING MODE IS NOT CORRECTLY SET! *******");
            assert ( 0 );
        }
    }
    else
#endif

    {
        addState( new StateBusy );
        addState( new StateCallEnded );
        addState( new StateDialing );
        addState( new StateError );
        addState( new StateFastBusy );
        addState( new StateIdle );
        addState( new StateInCall );
        addState( new StateRinging );
        addState( new StateTrying );
        addState( new StateBusyHere );
        addState( new StateOnhold );

		/* added by lizhijie, 2005.02.20 */
		addState( new StatePbxDialing );
		addState( new StatePbxRinging );
#if 0		
		addState( new StatePbxInCall );
#endif

        const UaXferMode xferMode = UaConfiguration::instance()->getXferMode();

        if ( xferMode != XferOff )
        {
            addState( new StateInCallDialing );
            addState( new StateReRinging );
            addState( new StateIn2Calls );
            addState( new StateBeingXferred );
            addState( new StateInCallBusy );
            addState( new StateInCallFastBusy );
            addState( new StateInCallTrying );

            if ( xferMode == XferOn )
            {
                addState( new StateBlindXferring );
                addState( new StateConsultXferring );
            }
            else if ( xferMode == ConfOn )
            {
                addState( new StateConferencing );
            }
        }

    }
}    // UaStateMachine::UaStateMachine



///
UaStateMachine::~UaStateMachine()
{}    // UaStateMachine::~UaStateMachine


void
UaStateMachine::process( const Sptr < SipProxyEvent > event )
{
    assert( event != 0 );

    Sptr<UaCallInfo> call;
    call.dynamicCast( event->getCallInfo() );
    assert( call != 0 );

	if (call == 0) return;
    cpDebug(LOG_DEBUG, "Current State is %s", call->getState()->name() );

    Sptr<State> nextState = call->getState()->process( event );

    if( nextState != PROXY_CONTINUE )
    {
	call->setState( nextState );
	cpDebug(LOG_DEBUG, "Next State is %s", nextState->name() );
	int i= 0;
   	while(strcmp(des[i].name, call->getState()->name()) != 0) 
		  
    	{
    		if (strcmp(des[i].name, "Unknow") != 0) return;
    		i++;
    	}

	char bufStr[128];
    	if(event->getDeviceId() < PSTN_DEVICES_EPOCH)
		sprintf(bufStr, "%s%d %s", "PHONE_", event->getDeviceId(),  des[i].describe);
	else if(event->getDeviceId()<VIRTUAL_DEVICES_EPOCH)
		sprintf(bufStr, "%s%d %s", "PSTN_", (event->getDeviceId()-PSTN_DEVICES_EPOCH),  des[i].describe);
	else
		sprintf(bufStr, "%s%d %s", "PBX_", (event->getDeviceId()-VIRTUAL_DEVICES_EPOCH),  des[i].describe);
    	UaDevice::instance()->reportDeviceInfo(event->getDeviceId(),ACTION_UNKNOW,bufStr);
    }
}    // UaStateMachine::process

///
Sptr < State >
UaStateMachine::findState( const string stateName )
{
    StateIter iter = myStates.begin();
    while ( iter != myStates.end() )
    {
        if ( stateName == (*iter)->name() )
        {
            return *iter;
        }
        iter++;
    }
    return 0;
}


///
const char* const
UaStateMachine::name() const
{
    return "User Agent State Machine";
}    // UaStateMachine::name


