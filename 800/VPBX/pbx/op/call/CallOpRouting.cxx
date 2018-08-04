/*
 * $Id: CallOpRouting.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "CallOpBuilder.hxx"

#include "PbxAgent.hxx"

using namespace Assist;

const Sptr <PbxState> CallOpRouting::process( Sptr <PbxEvent> _event )
{
//	call_cause_t err = CALL_CAUSE_NO_ERROR;

	if( checkCallMsg(_event) != CALL_SETUP )
	{
		return PBX_STATE_CONTINUE;
	}
	
	Sptr <_CallSetup> setupMsg = NULL;
	setupMsg.dynamicCast(isCallMsg( _event) );
	if(setupMsg==0) /* not the msg handled in this operator */
	{	
		cpLog(LOG_ERR, "No SETUP CallEvent in this callEvent");
		return PBX_STATE_CONTINUE;
	}

	Sptr <CallInfo> call = getCallInfo(_event);
 	if(call == 0)
		return PBX_STATE_CONTINUE;

	call_cause_t ecause = CALL_CAUSE_NO_ROUTE;
	Sptr <CallEvent> newCallEvent;
	Sptr <_CallRelease> rel;

	Sptr <CallEvent> event = isCallEvent(_event);// NULL;
	Sptr <EndPoint> dest = PbxAgent::instance()->getGateway()->findEPByNumber(setupMsg->getTo() );
	if ( dest != 0)
	{
		if (dest->getState()->getStateType() == 0)
		{
			goto CALLSETUP;
		}

		ecause = CALL_CAUSE_BUSY_HERE;
		cpLog(LOG_DEBUG, "Error: Ep with phonenum %s is busy", setupMsg->getTo());
	}
	if (ecause == CALL_CAUSE_NO_ROUTE)
		cpLog(LOG_DEBUG, "Error: Not found Ep with phonenum %s", setupMsg->getTo());

	newCallEvent = new CallEvent;
	rel = new _CallRelease;
	rel->setCause(ecause);
	newCallEvent->setCallMsg( rel);	/*replace the Sptr */
	//event->setEndPoint(event->getEndPoint());
	newCallEvent->setEndPoint(_event->getEndPoint());
	sendGatewayEvent(newCallEvent);

	return call->findState(CALL_STATE_IDLE);
	
CALLSETUP:
	/*when dest is not null, it must be free now */
	cpLog(LOG_DEBUG, "Found a EndPoint (%s) for destinational call", dest->getName() );

	/* set CallId of Dest EndPoint directly */
	dest->setCallId( call->getCallId() );
	call->setDestEP( dest);

	/* reply to src EP first */
#if 1	

	//CallEvent *newProceeding= new CallEvent();
	_CallProceeding *proceeding = new _CallProceeding();
//	proceeding->setCause(err );
	event->setCallMsg(proceeding);	/*replace the Sptr */
	sendGatewayEvent(_event);

#else
	_CallAlerting *alerting = new _CallAlerting();
//	proceeding->setCause(err );
	CallEvent *alertEvent = new CallEvent();
	
	alertEvent->setCallMsg(alerting);	
	alertEvent->setCallId(call->getSrcEP()->getCallId() );
	alertEvent->setEndPoint(call->getSrcEP());	
	sendGatewayEvent(alertEvent);
#endif

#if 0
	Sptr <DeviceMgr> temp = PbxAgent::instance()->getGateway()->findDeviceMgrByNumber(setupMsg->getTo() );
	assert(temp != 0);
	assert(temp->getQueue() != 0);
	/* forward SETUP to dest EP later */	
	CallEvent *newSetup = new CallEvent(temp->getQueue());
#endif

	/* forward SETUP to dest EP later */	
	CallEvent *newSetup = new CallEvent( );
	newSetup->setCallMsg(setupMsg );
//	newSetup->setCallId( call->getSrcEP()->getCallId() );
	newSetup->setCallInfo( call);
	newSetup->setEndPoint( dest);

	sendGatewayEvent( newSetup);

	return call->findState(CALL_STATE_PROCEEDING);
}


