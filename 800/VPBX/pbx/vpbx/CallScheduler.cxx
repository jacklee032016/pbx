/*
 * $Id: CallScheduler.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "global.h"

#include "PbxAgent.hxx"

#include "CallScheduler.hxx"
#include "CallContainer.hxx"
#include "PbxStateMachine.hxx"

#include "CallEvent.hxx"
#include "CallTimerEvent.hxx"

#include "PbxStateMachine.hxx"
#include "CallStateBuilder.hxx"

#ifdef API
//#include "api.h"
#include "intf.hxx"
#include "CommonMsg.hxx"
#include "CallMsgBuilder.hxx"
#endif
using namespace Assist;

CallScheduler::CallScheduler(PbxAgent *agent)
	: PbxManager(agent, "CallScheduler")
{
//	Sptr <CallScheduler> self = this;
	PbxStateMachine *stateMachine =  new PbxStateMachine(this);

	stateMachine->addState( new CallStateIdle );
	stateMachine->addState( new CallStateSetup );
	stateMachine->addState( new CallStateProceeding );
	stateMachine->addState( new CallStateAlerting );
	stateMachine->addState( new CallStateDisconnect );
	stateMachine->addState( new CallStateConnect );
//	stateMachine->addState( new CallStateError );

	setPbxStateMachine( stateMachine);

	myCalls = new CallContainer;
}

CallScheduler::~CallScheduler()
{
	cpLog(LOG_DEBUG, "Destorying Call Scheduler.....");
}

void CallScheduler::myMainLoop(void *params)
{/* no other operations are needed in CallScheduler */
	return;
}

void CallScheduler::myProcessEvent( const Sptr <PbxEvent> event )
{
	assert ( event != 0 );

	/* PbxTimerEvent, only PbxTimerEvent are used in CallScheduler thread 
	* This event must be process first
	*/
#if 0	
	Sptr <PbxTimerEvent > timerEvent = NULL;
	timerEvent.dynamicCast( event );

	if ( timerEvent != NULL )
	{
		if( processCallTimerEvent( timerEvent ) )
		{
			cpLog( LOG_DEBUG, "Returning from processCallTimerEvent\n" );
			return;
		}
		sendEvent(event );
		return;
	}
#endif

	Sptr <CallEvent> callEvent;
	callEvent.dynamicCast( event );

	if ( callEvent != 0 )
	{/** Returning 'true' from a 'process' or 'handle' method causes
	*  CallScheduler to exit without running sendEvent()	 */
#ifdef API
		
		api_msg_t *apiMsg = event2Struc(callEvent);

		intf_queue_put(myPutQ,apiMsg);

#else
		processCallEvent( callEvent );
		
		Sptr <CallInfo> call = NULL;
		Sptr <PbxStateObject> obj = callEvent->getStateObject();
		if(obj==NULL)
		{
			cpLog(LOG_ERR, "No CallInfo object has be assigned in this CallEvent");
			assert(0);
		}
		call.dynamicCast(obj);
		assert(call != 0);

		if(call->getState()->getStateType() == CALL_STATE_IDLE )
		{
			cpLog(LOG_DEBUG, "CallInfo in state of IDLE, so remove it");
			myCalls->removeCall(call->getCallId());
		}
#endif
		return;
	}

	cpLog( LOG_ERR, "Unknown Event in CallScheduler" );
	assert( 0 );
}

/*
*  find CallInfo for this callEvent and add this callInfo into CallEvent->StateObject
* then transfer to Call StateMachine process, so always return false;
*/
bool CallScheduler::processCallEvent( const Sptr <CallEvent> callEvent )
{
	/* EndPoint used to feedback */
	Sptr <EndPoint> ep = callEvent->getEndPoint();
	assert(ep != 0);
	cpLog( LOG_DEBUG, "Call Event from EndPoint %s", callEvent->getEndPoint()->getName() );

#if WITH_DEBUG	
	Sptr <CommonMsg> commMsg = callEvent->getCallMsg();
	assert(commMsg != 0);
	cpLog(LOG_DEBUG, "Call Msg is '%s'", commMsg->_Describer() );
#endif

	/* CallId of CallEvent used to retrieve CallInfo, so EndPoint can clear the reference of CallId at any time */
//	Sptr <CallId> callId = callEvent->getEndPoint()->getCallId();
	Sptr <CallId> callId = callEvent->getCallId();
	assert(callId != 0);	/* CallID is create in EndPoint state machine, must not be null */

	Sptr <CallInfo> call = myCalls->findCall( callId );

	if(call==0)
	{
		cpLog(LOG_DEBUG, "A new Call with CallID %s is created", callId->encode().logData()  );
		call = myCalls->getCall( callId );
		assert( call != 0 );
//		call->setState( getPbxStateMachine()->findState( CALL_STATE_SETUP ));
#ifndef API
		call->setState( getPbxStateMachine()->findState( CALL_STATE_IDLE ));
#endif
		call->setSrcEP(ep);
		/* added callId in CallInfo only for remove callInfo from callContainer */
		call->setCallId( callId);
	}
	callEvent->setStateObject(call);

#ifdef API
	myCallInfo = call;
#endif

	cpLog( LOG_DEBUG, "CallId %s ", callId->encode().logData()  );
	cpLog( LOG_DEBUG, "Current State is %s", call->getState()->name() );

	getPbxStateMachine()->process( callEvent );
	return false;
}  

bool CallScheduler::processCallTimerEvent( const Sptr <PbxTimerEvent> callTimerEvent )
{
	cpLog(LOG_DEBUG, "Removing CallInfo" );
	assert(callTimerEvent != 0 );
	
	Sptr <PbxStateObject> obj = callTimerEvent->getStateObject();
	assert( obj != 0 );
	
	Sptr <CallInfo> call = NULL;
	call.dynamicCast( obj);
	assert( call != 0);

	cpLog(LOG_DEBUG, "Removing CallInfo" );
	
#if 0
	if( call->getState()->getStateType() != CALL_STATE_ERROR)
	{	
	cpLog(LOG_DEBUG, "Removing CallInfo" );
//		cpLog( LOG_DEBUG, "CallInfo with CallId %s in state of %s", call->getCallId()->encode().logData(), call->getState()->name() );
		return true;
	}
#endif

	cpLog(LOG_DEBUG, "Removing CallInfo" );

//	cpLog( LOG_DEBUG, "CallInfo with CallId %s in state of %s will be remove", call->getCallId()->encode().logData(), call->getState()->name() );
	myCalls->removeCall( call->getCallId() );
	cpLog(LOG_DEBUG, "Removing CallInfo" );
	
	return true;
}

bool CallScheduler::handleCallWaiting(  const Sptr <PbxEvent > sipEvent   )
{
#if 0
	int deviceId = sipEvent->getDeviceId();

	Sptr<CallId> newCallId = new CallId(sipEvent->getSipMsg()->getCallId());

 	HardwareStatusType hdwStatus = DeviceMgrInstance::instance()->getHardwareStatus( deviceId );

	if(hdwStatus == HARDWARE_UNAVAILABLE)
	{
		cpLog( LOG_DEBUG, "New Invite for PBX real busy device");	
		sipEvent->setDeviceId(AS_DEVICE_NOT_DEFINED);
		callInfo = myCalls->newCall( *newCallId,  AS_DEVICE_NOT_DEFINED);
		assert( callInfo != 0 );
		callInfo->setFeature( myStateMachine );
		callInfo->setState( myStateMachine->findState( "StateBusyHere" ) );
		return false;
	}
	else if ( hdwStatus == HARDWARE_AVAILABLE )	
	{
		cpLog( LOG_DEBUG, "New Invite for PBX real idle device");	
		DeviceMgrInstance::instance()->setCallId( newCallId , deviceId );
		callInfo = myCalls->newCall( *newCallId, deviceId );
		assert( callInfo != 0 );
		callInfo->setFeature( myStateMachine );
		callInfo->setState( myStateMachine->findState( "IsdnStateIdle" ) );
		return false;
	}	
	else if ( hdwStatus == HARDWARE_CALLWAITING_ALLOWED )	
	{

		Sptr < CallId > activeId = DeviceMgrInstance::instance()->getCallId( deviceId );

		/// allow call waiting only if the current call is talking to one party
		if ( activeId != 0 )
		{
			Sptr < CallId > activeCall = myCalls->findCall( *activeId ,deviceId );
			if ( activeCall != 0 )
			{
				if ( activeCall->getContactList()->size() > 1 )
				{
					cpLog( LOG_DEBUG, "New Invite for PBX real busy device");	
					sipEvent->setDeviceId(AS_DEVICE_NOT_DEFINED);
					callInfo = myCalls->newCall( *newCallId,  AS_DEVICE_NOT_DEFINED);
					assert( callInfo != 0 );
					callInfo->setFeature( myStateMachine );
					callInfo->setState( myStateMachine->findState( "StateBusyHere" ) );
					return false;
				}
			}
		}

		cpLog( LOG_DEBUG, "New Invite for PBX real callwaiting device");	
		DeviceMgrInstance::instance()->addCallWaitingId( newCallId , deviceId );
		callInfo = myCalls->newCall( *newCallId, deviceId );
		assert( callInfo != 0 );
		callInfo->setFeature( myStateMachine );
		callInfo->setState( myStateMachine->findState( "IsdnStateIdle" ) );
		return false;
	}
	
#endif
	return true;
}

Sptr <CallContainer> CallScheduler::getCallContainer()
{
	return myCalls;
}

#ifdef  API
api_msg_t *  CallScheduler::event2Struc(const Sptr <CallEvent> callEvent)
{
	Sptr <EndPoint> ep=callEvent->getEndPoint();
	if (ep == 0) return NULL;

	Sptr <CommonMsg> cMsg = callEvent->getCallMsg();
	if ( cMsg == 0) return NULL;
//	string mName = cMsg->getMethodName() ;
	
	api_msg_t *apiMsg = (api_msg_t *)malloc(sizeof(api_msg_t));
	apiMsg->type = API_TYPE_MSG;
	apiMsg->intf_ep_index = ep->getIndex();
	apiMsg->intf_ep_name =(char *) ep->getName();
	apiMsg->intf_msg_method = (api_call_method_t)cMsg->getMethod();
	apiMsg->intf_from = (char *)cMsg->getFrom();
	apiMsg->intf_to = (char *)cMsg->getTo();
	apiMsg->intf_dev_index = ep->getCtrlDevice()->getIndex();

	if(cMsg->getMethod() == CALL_SETUP)
		processCallEvent( callEvent ) ;
	return apiMsg;
}

api_result_t  CallScheduler::struc2Event(api_msg_t * msg)
{
	assert(msg);

	Sptr <EndPoint> dest;


	if(msg->intf_ep_index < 0)
	{
		if(!msg->intf_to)
		{
			return  API_SEND_RESULT_ERROR;
		}
		dest = PbxAgent::instance()->getGateway()->findEPByNumber(msg->intf_to);
	}
	else
	{
		dest = PbxAgent::instance()->getGateway()->findEP(msg->intf_ep_index );			
	}
	
	if ( dest == 0)
	{
		return API_SEND_RESULT_NO_ROUTING;
	}

	cpLog(LOG_DEBUG, "Found a EndPoint (%s) for destinational call", dest->getName() );
	Sptr <DeviceMgr> temp;
	temp = dest->getCtrlDevice()->getParent();
		
	assert(temp != 0);
	assert(temp->getQueue() != 0);

	dest->setCallId(myCallInfo->getCallId());
	
	/* forward SETUP to dest EP later */	
	if(msg->intf_msg_method == API_CALL_SETUP)
	{
		if (dest->getState()->getStateType() != 0)
		{
			return API_SEND_RESULT_BUSY;
		}
		
		Sptr <_CallSetup> setupMsg = new _CallSetup;
		CallEvent *newSetup = new CallEvent(temp->getQueue());

		myCallInfo->setDestEP(dest);
		newSetup->setCallInfo(myCallInfo);

//		setupMsg->setTo(dest->getCtrlDevice()->getPhoneNumber());
		setupMsg->setTo(dest->getPhoneNumber());
		setupMsg->setFrom("01");
		newSetup->setCallMsg(setupMsg );
		newSetup->setEndPoint( dest);
		
		PbxAgent::instance()->getGateway()->reportEvent(newSetup);
	}

	else if(msg->intf_msg_method == API_CALL_PROCEEDING)
	{
		// first to process  in error state, and report result, refer to SETUP

		Sptr <_CallProceeding> proMsg = new _CallProceeding;
		CallEvent *newProceeding = new CallEvent(temp->getQueue());

		myCallInfo->setDestEP(dest);
		newProceeding->setCallInfo(myCallInfo);
		
		newProceeding ->setCallMsg(proMsg );
		newProceeding ->setEndPoint( dest);
		
		PbxAgent::instance()->getGateway()->reportEvent(newProceeding );

	}
	else if(msg->intf_msg_method == API_CALL_ALERTING)
	{
		// first to process  in error state, and report result, refer to SETUP

		Sptr <_CallAlerting> alertMsg = new _CallAlerting;
		CallEvent *newAlert = new CallEvent(temp->getQueue());

		myCallInfo->setDestEP(dest);
		newAlert->setCallInfo(myCallInfo);
		
		newAlert->setCallMsg(alertMsg );
		newAlert->setEndPoint( dest);
		
		PbxAgent::instance()->getGateway()->reportEvent(newAlert);

	}
	else if(msg->intf_msg_method == API_CALL_CONNECT)
	{
		// first to process  in error state, and report result, refer to SETUP

		Sptr <_CallConnect> connMsg = new _CallConnect;
		CallEvent *newConnect = new CallEvent(temp->getQueue());

		myCallInfo->setDestEP(dest);
		newConnect->setCallInfo(myCallInfo);
		
		newConnect->setCallMsg(connMsg );
		newConnect->setEndPoint( dest);
		
		PbxAgent::instance()->getGateway()->reportEvent(newConnect);

	}
	else if(msg->intf_msg_method == API_CALL_CONNECT_ACK)
	{
		
		// first to process  in error state, and report result, refer to SETUP

		
		Sptr <_CallRelease> sendMsg = new _CallRelease;
		CallEvent *callEvent = new CallEvent(temp->getQueue());

		myCallInfo->setDestEP(dest);
		callEvent->setCallInfo(myCallInfo);
		
		callEvent->setCallMsg(sendMsg );
		callEvent->setEndPoint( dest);
		
		PbxAgent::instance()->getGateway()->reportEvent(callEvent);
	}

	else if(msg->intf_msg_method == API_CALL_DISCONNECT)
	{
		// first to process  in error state, and report result, refer to SETUP

		
		Sptr <_CallDisConnect> sendMsg = new _CallDisConnect;
		CallEvent *callEvent = new CallEvent(temp->getQueue());

		myCallInfo->setDestEP(dest);
		callEvent->setCallInfo(myCallInfo);
		
		callEvent->setCallMsg(sendMsg );
		callEvent->setEndPoint( dest);
		
		PbxAgent::instance()->getGateway()->reportEvent(callEvent);

	}
#if 1
	else if(msg->intf_msg_method == API_CALL_RELEASE)
	{
		
		// first to process  in error state, and report result, refer to SETUP

		
		Sptr <_CallRelease> sendMsg = new _CallRelease;
		CallEvent *callEvent = new CallEvent(temp->getQueue());

		myCallInfo->setDestEP(dest);
		callEvent->setCallInfo(myCallInfo);
		
		callEvent->setCallMsg(sendMsg );
		callEvent->setEndPoint( dest);
		
		PbxAgent::instance()->getGateway()->reportEvent(callEvent);
	}
	else if(msg->intf_msg_method == API_CALL_RELEASE_COMPLETE)
	{
		
		// first to process  in error state, and report result, refer to SETUP

		//nothing to do after getting release_complete	
	
	}
#endif
	return API_SEND_RESULT_OK;
}

void CallScheduler::set_queue_put(api_queue_t *queue)
{
	myPutQ = queue;
}
#endif

