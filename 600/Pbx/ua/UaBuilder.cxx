/*
 * $Id: UaBuilder.cxx,v 1.8 2007/09/03 06:50:41 lizhijie Exp $
 */

#include "global.h"
#include "SipEvent.hxx"
#include "StatusMsg.hxx"
#include "SipCommand.hxx"

#include "UaBuilder.hxx"
#include "UaCallContainer.hxx"
#include "UaStateMachine.hxx"
#include "UaDeviceEvent.hxx"
#include "UaDevice.hxx"
#include "UaDigitTimerEvent.hxx"
#include "UaTimerEvent.hxx"
#include "UaConfiguration.hxx"

#include "AssistNullDevice.hxx"
#include "SipAllow.hxx"

using namespace Vocal;
Sptr < RegistrationManager > UaBuilder::registrationManager = 0;

UaBuilder::UaBuilder()
{
	Sptr <UaStateMachine> uaStateMachine = new UaStateMachine;
	addFeature( uaStateMachine );
	initFeature();
}    // UaBuilder::UaBuilder

UaBuilder::~UaBuilder()
{}    // UaBuilder::~UaBuilder


void UaBuilder::process( const Sptr <SipProxyEvent> nextEvent )
{
	assert ( nextEvent != 0 );
	assert ( myCallContainer != 0 );

	//calls.dynamicCast( myCallContainer );
	//assert( calls != 0 );

	//stateMachine.dynamicCast( *( myFeatures.begin() ) );
	//assert( stateMachine != 0 );

	/// Handle the 4 types of events:

	/// SipEvent
	Sptr <SipEvent> sipEvent;
	sipEvent.dynamicCast( nextEvent );
	if ( sipEvent != 0 )
	{
		/** Returning 'true' from a 'process' or 'handle' method causes
		*  UaBuilder to exit without running sendEvent()
		*/
		if( processSipEvent( sipEvent ) )
		{
			cpDebug(LOG_DEBUG, "Returning from processSipEvent\n" );
			return;
		}
		sendEvent( nextEvent );
		return;
	}

	/// UaDeviceEvent
	Sptr <UaDeviceEvent> uaDeviceEvent;
	uaDeviceEvent.dynamicCast( nextEvent );
	if ( uaDeviceEvent != 0 )
	{
		if( processUaDeviceEvent( uaDeviceEvent ) )
		{
			cpDebug(LOG_DEBUG, "Returning from processUaDeviceEvent\n" );
			return;
		}
		sendEvent( nextEvent );
		return;
	}

	/// UaDigitEvent
	Sptr < UaDigitTimerEvent > uaDigitEvent;
	uaDigitEvent.dynamicCast( nextEvent );
	if ( uaDigitEvent != 0 )
	{
		if( processUaDigitEvent( uaDigitEvent ) )
		{
			cpDebug(LOG_DEBUG, "Returning from processUaDigitEvent\n" );
			return;
		}
		sendEvent( nextEvent );
		return;
	}

	/// UaTimerEvent
	Sptr < UaTimerEvent > uaTimerEvent;
	uaTimerEvent.dynamicCast( nextEvent );
	if ( uaTimerEvent != 0 )
	{
		if( processUaTimerEvent( uaTimerEvent ) )
		{
			cpDebug(LOG_DEBUG, "Returning from processSipEvent\n" );
			return;
		}
		sendEvent( nextEvent );
		return;
	}

	/// Event of unknown type
	// we shouldn't get to here.
	cpLog( LOG_ERR, "Unknown Event" );
	assert( 0 );

}

/**
 * A parent 'process' method
 */
bool UaBuilder::processSipEvent( const Sptr < SipEvent > sipEvent )
{
#if 0 //modified by lijie 2005-06-03
//	int deviceId;

	cpDebug(LOG_DEBUG, "SIP message event In processSipEvent()" );

	Sptr < StatusMsg > statusMsg;
	statusMsg.dynamicCast( sipEvent->getSipMsg() );

	/// If we have a status msg, we handle it
	if ( statusMsg != 0 )
	{
		if( handleStatusMsg( sipEvent ) )
		{
			return true;
		}
	}
#if 0
    Sptr < SipEvent > sipEvent;
    sipEvent.dynamicCast( event );
    if ( sipEvent == 0 )
    {
        return 0;
    }
    Sptr < SipMsg > sipMsg = sipEvent->getSipMsg();
    assert( sipMsg != 0 );

    Sptr < StatusMsg > msg;
    msg.dynamicCast( sipMsg );
#endif
    /// If we get here, we have a SIP message event
	cpDebug(LOG_DEBUG, "SIP message event" );

	/* added by lizhijie ,2004.02.12, 02.17 */
	Sptr <SipCallLeg> sipCallLeg;
	sipCallLeg = sipEvent->getSipCallLeg();
	callId = sipCallLeg->getCallId();

	string calleenum, callernum;
	AssistDeviceInfo  *dev;
	if ( statusMsg != 0 )
	{/* caller */
		callernum= sipCallLeg->getFrom().getUser().convertString();
		cpDebug(LOG_DEBUG, "Msg for Caller , Caller num is %s", callernum.c_str() );
//		dev = (AssistDeviceInfo *)UaDevice::instance()->getDigitCollector( sipEvent->getDeviceId() )->checkHardwareForLocalDial(callernum);
		dev = (AssistDeviceInfo *)UaDevice::instance()->getAssistDeviceInfoByPhoneNumber( callernum.c_str() );
	}
	else
	{/* callee*/
		calleenum= sipCallLeg->getTo().getUser().convertString();
		cpDebug(LOG_DEBUG, "Msg for Callee , Callee num is %s", calleenum.c_str() );
//		dev = (AssistDeviceInfo *)UaDevice::instance()->getDigitCollector( sipEvent->getDeviceId() )->checkHardwareForLocalDial(calleenum);
		dev = (AssistDeviceInfo *)UaDevice::instance()->getAssistDeviceInfoByPhoneNumber( calleenum.c_str() );
	}

	if(dev!=0)
	{
		sipEvent->setDeviceId( dev->getMyDevice()->id );
		cpDebug(LOG_DEBUG, " From user: %s, To user: %s(device %s)",  sipCallLeg->getFrom().getUser().logData(), calleenum.c_str() ,dev->getMyDevice()->name );
	}	
	else
	{
	
		cpDebug(LOG_DEBUG, "SIP message for PBX virtual device");
//		int deviceId = UaDevice::instance()->getPbxDeviceId( callId);
		int deviceId = UaDevice::instance()->getDeviceId( callId);

		Sptr < InviteMsg > invitemsg;
		invitemsg.dynamicCast( sipEvent->getSipMsg() );

		if(deviceId == AS_DEVICE_NOT_DEFINED 
			&& invitemsg != 0 )
		{
			cpDebug(LOG_DEBUG, "This is a new call to PBX virtual device");
			deviceId = UaDevice::instance()->getFreePbxDevice();
			if( deviceId == AS_DEVICE_NOT_DEFINED )
			{
				cpDebug(LOG_DEBUG, "Busy here for new call to PBX virtual device");
				//added by lijie		
				callInfo = calls->newCall( sipEvent->getSipCallLeg()->getCallId() , AS_DEVICE_NOT_DEFINED);
				callInfo->setFeature( stateMachine );
				callInfo->setState( stateMachine->findState( "StateBusyHere" ) );
				return false;
				//added ends here
			}
			else
			{
				cpDebug(LOG_DEBUG, "New call for PBX is received, and pbxCallId is added for this virtual device %d" ,deviceId );
				sipEvent->setDeviceId( deviceId );
#if 0
				Sptr < SipCallId > newCallId = new SipCallId;
				Sptr <CallInfo> call_info;

				call_info = calls->newCall( *newCallId, deviceId );
				assert( call_info != 0 );
				call_info->setFeature( stateMachine );
				call_info->setState( stateMachine->findState( "StateIdle" ) );
				UaDevice::instance()->setPbxCallId( newCallId, deviceId );
				cpDebug(LOG_DEBUG, "New CallId for PBX callId %s with deviceId %d", newCallId->encode().logData() , deviceId );
				cpDebug(LOG_DEBUG, "State of new PBX callInfo is %s", call_info->getState()->name() );
#endif				
			}
		}
		else
		{
			sipEvent->setDeviceId( deviceId );
		}
	}
//	deviceId = (UaDevice::instance())->getDeviceId(callId);
//	sipEvent->setDeviceId(deviceId);
	/* end of added */

   /// Let's check the call info, now
	callInfo = calls->findCall( callId ,sipEvent->getDeviceId() );

	if ( callInfo == 0 )
	{
		cpDebug(LOG_DEBUG, "No CallInfo found" );

		Sptr < InviteMsg > inviteMsg;
		inviteMsg.dynamicCast( sipEvent->getSipMsg() );
		if ( inviteMsg == 0 )
		{
			cpLog (LOG_DEBUG, "Not an INVITE - Discard event" );
			return true;
		}
		else
		{
			cpDebug(LOG_DEBUG, "New INVITE" );
#if 0
			Sptr <SipCallLeg> sipCallLeg;
			sipCallLeg = sipEvent->getSipCallLeg();
			string calleenum = sipCallLeg->getTo().getUser().convertString();

			AssistDeviceInfo  *dev = (AssistDeviceInfo *)UaDevice::instance()->getDigitCollector( sipEvent->getDeviceId() )->checkHardwareForLocalDial(calleenum);
			if(dev!=0)
			{
				sipEvent->setDeviceId( dev->getMyDevice()->id );
				cpDebug(LOG_DEBUG, " From user: %s, To user: %s(device %s)",  sipCallLeg->getFrom().getUser().logData(), calleenum.c_str() ,dev->getMyDevice()->name );
			}	
#endif
			callInfo = calls->newCall( sipEvent->getSipCallLeg()->getCallId() , sipEvent->getDeviceId() );
			assert( callInfo != 0 );
			callInfo->setFeature( stateMachine );
#if 0
			if ( UaConfiguration::instance()->getLoadGenOn() )
			{
				/// Assume this is a new call...
				/// Also assume that we are not in use.
				callInfo->setState( stateMachine->findState( "StateAutoIdle" ) );
			}
			else  // LoadGen is off
#endif				
			{
				/// We're not in loadgen mode
				/// Handle call waiting conditions
				if( handleCallWaiting( callInfo, sipEvent->getDeviceId()  ) )
				{
					cpLog( LOG_ERR, "Returned from handleCallWaiting\n" );
					return true;
				}
			}
		} // lots of brackets!
	}

	return false;
#else

	cpDebug(LOG_DEBUG, "SIP message event In processSipEvent()" );

	Sptr < StatusMsg > statusMsg;
	statusMsg.dynamicCast( sipEvent->getSipMsg() );

	/// If we have a status msg, we handle it
	if ( statusMsg != 0 )
	{
		if( handleStatusMsg( sipEvent ) )
		{
			return true;
		}
	}

      /// If we get here, we have a SIP message event
	cpDebug(LOG_DEBUG, "SIP message event" );

	SipCSeq cseq = sipEvent->getSipMsg()->getCSeq();
	Data method = cseq.getMethod();
	method.uppercase();
	//cpDebug(LOG_DEBUG, "method------------: %s", method.logData());
	
	/* added by wuaiwu, 2005/11/1 */
	if ( (method != "INVITE" ) &&  
	     (method != "ACK") &&
	     ( method != "CANCEL") && 
	     (method != "BYE")
	   )
	{
		Sptr < SipCommand > scomm;
		scomm.dynamicCast( sipEvent->getSipMsg() );
		if (scomm != 0)
		{
			StatusMsg sallow(*scomm, 405);
			SipAllow allow("INVITE,ACK,CANCEL,BYE");
			sallow.setAllow(allow);
			sipEvent->getSipStack()->sendReply(sallow);
		}
		
		return true;
	}
	/* end of added */
	/* added by lizhijie ,2004.02.12, 02.17 */
	Sptr <SipCallLeg> sipCallLeg;
	sipCallLeg = sipEvent->getSipCallLeg();
	callId = sipCallLeg->getCallId();
	Sptr<SipCallId> newCallId = new SipCallId(callId);

	string calleenum, callernum, num;

	AssistDevice  *dev;

	int deviceId = AS_DEVICE_NOT_DEFINED;
	
	if(method == "BYE")
	{

		if(statusMsg == 0)
			num = sipCallLeg->getTo().getUser().convertString();
		else
			num = sipCallLeg->getFrom().getUser().convertString();
				
		dev = (AssistDevice *)UaDevice::instance()->getAssistDeviceByPhoneNumber( num.c_str(),  callId );

		
		if(dev!=0 )
		{
			deviceId = dev->getDeviceId();
		}

		callInfo = calls->findCall( callId ,deviceId);
		
		if ( callInfo != 0 )
		{
			sipEvent->setDeviceId(deviceId);
			return false;
		}

#if  0	
		callInfo = calls->findCall( callId, AS_DEVICE_NOT_DEFINED);
		
		if(callInfo != 0)
		{
			sipEvent->setDeviceId(AS_DEVICE_NOT_DEFINED);
			return false;
		}
#endif		
		//no invalid call info found
		return true;
			
	}
	else if(statusMsg!=0)
	{
		callernum = sipCallLeg->getFrom().getUser().convertString();
		dev = (AssistDevice *)UaDevice::instance()->getAssistDeviceByPhoneNumber( callernum.c_str(), callId );

		if(dev!=0 )
		{
			deviceId = dev->getDeviceId();
		}
		else
		{
			return true;
		}

#if 0
		/* added by lizhijie, 2007.09.02 */
		dev->setCallerId( sipCallLeg->getFrom().getUser().convertString() );
		cpLog(LOG_ERR, "CallerId  %s ", sipCallLeg->getFrom().getUser().convertString().c_str());
#endif
			
		callInfo = calls->findCall( callId ,deviceId);
		
		if(callInfo != 0)
		{
			sipEvent->setDeviceId(deviceId);
			return false;
		}
		//no invalid call info found
		return true;
	}
	else
	{
		//when the callinfo's statemachine is StateBusyHere
		callInfo = calls->findCall( callId, AS_DEVICE_NOT_DEFINED);
		
		if(callInfo != 0)
		{
			sipEvent->setDeviceId(AS_DEVICE_NOT_DEFINED);
			return false;
		}
			
		Sptr < InviteMsg > invitemsg;
		invitemsg.dynamicCast( sipEvent->getSipMsg() );
			
		if(sipCallLeg->getTo().getUser() == sipCallLeg->getFrom().getUser())
		{
			if(invitemsg!=0)
			{
				cpDebug(LOG_DEBUG, "Busy here for new call to device itself");
				sipEvent->setDeviceId(AS_DEVICE_NOT_DEFINED);
				callInfo = calls->newCall( callId , AS_DEVICE_NOT_DEFINED);
				callInfo->setFeature( stateMachine );
				callInfo->setState( stateMachine->findState( "StateBusyHere" ) );
				return false;
			}	

			return true;
		}		
		calleenum = sipCallLeg->getTo().getUser().convertString();
		dev = (AssistDevice *)UaDevice::instance()->getAssistDeviceByPhoneNumber( calleenum.c_str(), callId );

		if(dev!=0)
		{
		
			deviceId = dev->getDeviceId();
			
			/* added by lizhijie, 2007.09.02 */
			dev->setCallerId( sipCallLeg->getFrom().getUser().convertString() );

			cpDebug(LOG_INFO, "CallerId  %s , callee : %s", sipCallLeg->getFrom().getUser().convertString().c_str(), calleenum.c_str());

			callInfo = calls->findCall( callId, deviceId);
			if(callInfo != 0)
			{
				sipEvent->setDeviceId(deviceId);
				return false;
			}
			else
			{
				if(invitemsg!=0)
				{
					sipEvent->setDeviceId(deviceId);
					return handleCallWaiting(sipEvent);
				}
				
				return true;
			}
					
		}
		else
		{	
			//no device assicoated with the request msg
#if 0		
			if(invitemsg != 0)
			{
				cpDebug(LOG_DEBUG, "Busy here for new call to the same UA's Exchange");
				sipEvent->setDeviceId(AS_DEVICE_NOT_DEFINED);
				callInfo = calls->newCall( callId , AS_DEVICE_NOT_DEFINED);
				callInfo->setFeature( stateMachine );
				callInfo->setState( stateMachine->findState( "StateBusyHere" ) );
				return false;
			}
#endif			

			return true;
		}
	}


#endif
}  /// UaBuilder::processSipEvent


/**
 * A parent 'process' method
 */
bool UaBuilder::processUaDeviceEvent( const Sptr < UaDeviceEvent > uaDeviceEvent )
{
	cpDebug(LOG_DEBUG, "Device Event" );
    /// The following situtation can occur:
    ///   1.  user goes onhook
    ///   2.  user goes offhook immediately
    ///   3.  state machine processes the onhook and removes callid
    ///   4.  state machine processes the offhook with original callid
    ///   5.  the original callid no longer exists
    /// In this case, we will just change the callId in deviceEvent
    /// to the one in the device
    /// Short term fix only.

	bool exit = false;  /// Tells us whether or not to exit UaBuilder
//	Sptr <CallInfo> call_info;
	cpDebug(LOG_DEBUG, "Device Event" );
#if 0	
	if ( !(uaDeviceEvent->callId == UaDevice::instance()->getCallId(uaDeviceEvent->getDeviceId() )) )
	{
		uaDeviceEvent->callId = UaDevice::instance()->getCallId(uaDeviceEvent->getDeviceId());
	}
#endif	

	//added by lijie 2005-06-03
	uaDeviceEvent->callId = UaDevice::instance()->getCallId(uaDeviceEvent->getDeviceId());
	//added ends here	
	
	if ( uaDeviceEvent->callId == 0 )
	{
		if (  ( uaDeviceEvent->type == DeviceEventHookUp ) )
		{
			//added by lijie, pbx device can not hookup. 2005-08-25
			if(uaDeviceEvent->getDeviceId() >= VIRTUAL_DEVICES_EPOCH)
				return true;
			//added ends here
			cpDebug(LOG_DEBUG, "User Offhook" );
			Sptr < SipCallId > newCallId = new SipCallId;

#if 0
/* not used : callId is created in SipStack,lizhijie  */
			if ( UaConfiguration::instance()->getCallIdOn() )
			{// Use callId from config file
				Data newId( UaConfiguration::instance()->getCallId() );
				newCallId->setLocalId( newId );
			}
#endif 
			callInfo = calls->newCall( *newCallId, uaDeviceEvent->getDeviceId());
			assert( callInfo != 0 );

			callInfo->setFeature( stateMachine );
			callInfo->setState( stateMachine->findState( "StateIdle" ) );
			UaDevice::instance()->setCallId( newCallId ,uaDeviceEvent->getDeviceId() );
			uaDeviceEvent->setSipStack( sipStack );
		}
		else			// We have no callId & the event is NOT an offhook
		{
			cpLog( LOG_ERR, "Missing Call Id for hardware event" );
			exit = true;
			return exit;
		}
	}
	// We must have a valid callId to enter this else statement
	else
	{
		cpDebug(LOG_DEBUG, "Device Event with deviceId %d",uaDeviceEvent->getDeviceId() );
		callInfo = calls->findCall( *(uaDeviceEvent->callId), uaDeviceEvent->getDeviceId() );
		uaDeviceEvent->setSipStack( sipStack );
		if(callInfo==0)
		{
			int virtual_device_id = UaDevice::instance()->getPbxDeviceId(*(uaDeviceEvent->callId) );
			//added by lijie 2005-06-03
			uaDeviceEvent->setDeviceId(virtual_device_id);
			callInfo = calls->findCall( *(uaDeviceEvent->callId), virtual_device_id);
		}
			
		assert( callInfo != 0 );
		cpDebug(LOG_DEBUG, "CallId %s ", uaDeviceEvent->callId->encode().logData()  );
		cpDebug(LOG_DEBUG, "Current State is %s", callInfo->getState()->name() );
	}

	return exit;
}

/**
 * A parent 'process' method
 */
bool UaBuilder::processUaDigitEvent( const Sptr < UaDigitTimerEvent >
				   uaDigitEvent )
{
    cpDebug(LOG_DEBUG, "ua Digit Event" );
    if ( uaDigitEvent->callId == 0 )
    {
#if 0    
	if ( UaConfiguration::instance()->getLoadGenOn() )
	{
	    if (uaDigitEvent->getLoadGenSigType() == LoadGenStartCall)
	    {
		cpDebug(LOG_DEBUG, ">>> New Call! <<<" );
		Sptr < SipCallId > newCallId = new SipCallId;
		uaDigitEvent->callId = newCallId;
		
				callInfo = calls->newCall( *newCallId, uaDigitEvent->getDeviceId() );

		assert( callInfo != 0 );
		callInfo->setFeature( stateMachine );
		
		if (UaConfiguration::instance()->getRunMode() == "Calling" )
		{
		    callInfo->setState
			( stateMachine->findState( "StateAutoIdle" ) );
		}
		else if( UaConfiguration::instance()->getRunMode() == "RSTest")
		{
		    callInfo->setState
			( stateMachine->findState( "StateAutoRS" ) );
		}
		uaDigitEvent->setSipStack( sipStack );
	    }
	}
	else  // Loadgen is off
#endif		
	{
	    cpDebug(LOG_DEBUG, "Discarded" );
	    return true;
	}
    }
    else  // uaDigitEvent->callId != 0 
    {
    		callInfo = calls->findCall( *(uaDigitEvent->callId), uaDigitEvent->getDeviceId() );
			
	if ( callInfo == 0 )
	{
	    cpDebug(LOG_DEBUG, "Discard late timer event" );
	    return true;
	}
	uaDigitEvent->setSipStack( sipStack );
    }
    return false;
}

/**
 * A parent 'process' method
 */
bool
UaBuilder::processUaTimerEvent( const Sptr < UaTimerEvent > uaTimerEvent )
{
    Sptr < SipCallId > callId = uaTimerEvent->getCallId();
    assert( callId != 0 );

	callInfo = calls->findCall( *callId , uaTimerEvent->getDeviceId() );
	
    //assert( callInfo != 0 );
    //assert(sipStack !=0);

    /* modify by wuaiwu */
    if (callInfo != 0 && sipStack !=0)
   {
   	uaTimerEvent->setSipStack( sipStack );
    	return false;
   }
   else
   {
   	return true;
   }
}

/**
 * A child 'handle' method
 */
bool UaBuilder::handleStatusMsg( const Sptr <SipEvent> sipEvent )
{
	cpDebug(LOG_DEBUG, "In handleStatusMsg()" );
	/// Pull the StatusMsg out of the SipEvent
	Sptr < StatusMsg > statusMsg;
	statusMsg.dynamicCast( sipEvent->getSipMsg() );
	/// Sanity check; this checked out ok in main process method
	assert( statusMsg != 0 );
	/// Register
	int RegCode = 200;
	if ( UaConfiguration::instance()->getRegisterOn() && statusMsg->getCSeq().getMethod() == "REGISTER" )
	{
		if ( statusMsg->getStatusLine().getStatusCode() < 200 )
		{
			/// just return, prevents re-registration loop
			cpDebug(LOG_DEBUG, "REGISTER response: code %d", statusMsg->getStatusLine().getStatusCode() );
			return true;
		}

		string num = statusMsg->getTo().getUser().convertString();
		cpDebug(LOG_DEBUG, "num = %s", num.c_str());
		
		AssistDevice *dev = (AssistDevice *)UaDevice::instance()->getAssistDeviceByPhoneNumber( num.c_str(),  callId );
		int deviceId = dev->getDeviceId();
#if 0		
		if ((deviceId == -1) && num.substr(num.size()-4, 4) == "0000")
			deviceId = 1000;
#else
		/* added 2007.03.08 */
		if ((deviceId == -1) && num.substr(num.size()-4, 4) == UaConfiguration::instance()->getConsolePhone().c_str())
			deviceId = 1000;
#endif

		RegCode =  registrationManager->handleRegistrationResponse( *statusMsg );
//		cout << "RegCode = " << RegCode << endl;
		if ( RegCode == 200)
		{
#if 0		
			//cout << VTime::strftime("%y/%m/%d %H:%M:%S")
			cout  << " Registration OK" << endl;
			cout << "Ready" << endl;
#endif			
			cpDebug(LOG_DEBUG, " Registration OK");
			UaDevice::instance()->reportDeviceInfo(deviceId, ACTION_UNKNOW, "Regist OK");

		}
		else if (RegCode == 403)
		{
			// cpLog( LOG_WARNING, "Registration status code %d",
			//                       statusMsg->getStatusLine().getStatusCode() );
//			cout << "Registration forbidden" << endl;
			cpLog(LOG_WARNING, "Registration forbidden");
			UaDevice::instance()->reportDeviceInfo(deviceId, ACTION_UNKNOW, "Regist Forbidden");
		}
		
		return true;
	}

	
	/// Subscribe
	if ( UaConfiguration::instance()->getSubscribeOn() &&
		statusMsg->getCSeq().getMethod() == "SUBSCRIBE" )
	{
		if ( subManager == 0 )
		{
			cpLog( LOG_ERR, "SubscribeManager is not running" );
		}
		else
		{
			if ( subManager->handleSubscribeResponse( *statusMsg ) )
				cpLog( LOG_ERR, "Subscribe Ok" ); //cout << "Subscribe Ok" << endl;
			else
				cpLog( LOG_ERR, "Subscribe failed" );
		}
		return true;
	}
	
	/// Notify
	if ( UaConfiguration::instance()->getSubscribeOn() )
	{
		Sptr < NotifyMsg > notifyMsg;
		notifyMsg.dynamicCast( sipEvent->getSipMsg() );
		if ( notifyMsg != 0 )
		{
			subManager->handleNotify( *notifyMsg );
		}
	}
	return false; // a return to the calling method
} /// end handleStatusMsg()


/**
 * A child 'handle' method
 */
bool
UaBuilder::handleCallWaiting(  const Sptr < SipEvent > sipEvent   )
{
#if 0 //modified by lijie 2005-06-03
	if ( UaConfiguration::instance()->getCallWaitingOn() )
	{
		cpDebug(LOG_DEBUG, "Call Waiting ON" );
		
		/// if call waiting enabled, the new call will ring
		/// the set even when there is an active call
		HardwareStatusType hdwStatus = UaDevice::instance()->getHardwareStatus(deviceId);
#if 0
	if ( hdwStatus == HARDWARE_UNAVAILABLE )
	{
	    cpDebug(LOG_DEBUG, "Hardware Unavailabe" );
	    /// Device is in use
	    callInfo->setState( stateMachine->findState( "StateBusyHere" ) );
	}
	else
	{
#endif
		string nextState = "StateIdle";

		if ( hdwStatus == HARDWARE_AVAILABLE )
		{
			cpDebug(LOG_DEBUG, "Hardware Available" );
		}
		else
		{
			cpDebug(LOG_DEBUG, "Hardware available for call waiting only" );
		}

		Sptr < SipCallId > activeId = UaDevice::instance()->getCallId( deviceId );

		/// allow call waiting only if the current call is talking to one party
		if ( activeId != 0 )
		{
			Sptr < UaCallInfo > activeCall = calls->findCall( callId ,deviceId );
			if ( activeCall != 0 )
			{
				if ( activeCall->getContactList()->size() > 1 )
					nextState = "StateBusyHere";
			}
		}
		// New Call
		callInfo->setState( stateMachine->findState( nextState ) );
// bug:
//	    callInfo->setState( stateMachine->findState( "StateIdle" ) );
#if 0
	}
#endif
	}
	/// Call Waiting is NOT turned on:
	else
	{
		cpDebug(LOG_DEBUG, "Call Waiting Off" );

		/// if call waiting is disabled, the new call will not ring
		/// the set when there is an active call
		if ( UaDevice::instance()->getHardwareStatus( deviceId) == HARDWARE_AVAILABLE )
		{
			cpDebug(LOG_DEBUG, "Hardware Available" );
			// New Call
			callInfo->setState( stateMachine->findState( "StateIdle" ) );
		}
		else
		{
			cpDebug(LOG_DEBUG, "Hardware Unavailable" );
			// Device is in use
			callInfo->setState( stateMachine->findState( "StateBusyHere" ) );
		}
	}
	return false;
#else
	int deviceId = sipEvent->getDeviceId();

	Sptr<SipCallId> newCallId = new SipCallId(sipEvent->getSipMsg()->getCallId());

 	HardwareStatusType hdwStatus = UaDevice::instance()->getHardwareStatus( deviceId );

	if(hdwStatus == HARDWARE_UNAVAILABLE)
	{
		cpDebug(LOG_DEBUG, "New Invite for PBX real busy device");	
		sipEvent->setDeviceId(AS_DEVICE_NOT_DEFINED);
		callInfo = calls->newCall( *newCallId,  AS_DEVICE_NOT_DEFINED);
		assert( callInfo != 0 );
		callInfo->setFeature( stateMachine );
		callInfo->setState( stateMachine->findState( "StateBusyHere" ) );
		return false;
	}
	else if ( hdwStatus == HARDWARE_AVAILABLE )	
	{
		cpDebug(LOG_DEBUG, "New Invite for PBX real idle device");	
		UaDevice::instance()->setCallId( newCallId , deviceId );
		callInfo = calls->newCall( *newCallId, deviceId );
		assert( callInfo != 0 );
		callInfo->setFeature( stateMachine );
		callInfo->setState( stateMachine->findState( "StateIdle" ) );
		return false;
	}	
	else if ( hdwStatus == HARDWARE_CALLWAITING_ALLOWED )	
	{

		Sptr < SipCallId > activeId = UaDevice::instance()->getCallId( deviceId );

		/// allow call waiting only if the current call is talking to one party
		if ( activeId != 0 )
		{
			Sptr < UaCallInfo > activeCall = calls->findCall( *activeId ,deviceId );
			if ( activeCall != 0 )
			{
				if ( activeCall->getContactList()->size() > 1 )
				{
					cpDebug(LOG_DEBUG, "New Invite for PBX real busy device");	
					sipEvent->setDeviceId(AS_DEVICE_NOT_DEFINED);
					callInfo = calls->newCall( *newCallId,  AS_DEVICE_NOT_DEFINED);
					assert( callInfo != 0 );
					callInfo->setFeature( stateMachine );
					callInfo->setState( stateMachine->findState( "StateBusyHere" ) );
					return false;
				}
			}
		}

		cpDebug(LOG_DEBUG, "New Invite for PBX real callwaiting device");	
		UaDevice::instance()->addCallWaitingId( newCallId , deviceId );
		callInfo = calls->newCall( *newCallId, deviceId );
		assert( callInfo != 0 );
		callInfo->setFeature( stateMachine );
		callInfo->setState( stateMachine->findState( "StateIdle" ) );
		return false;
	}
	
	return true;
#endif
}

void UaBuilder::sendEvent( const Sptr < SipProxyEvent > nextEvent )
{
	nextEvent->setCallInfo ( callInfo, calls );
	stateMachine->process( nextEvent );

	return;
}

void UaBuilder::sendEvent( const Sptr < SipProxyEvent > nextEvent , Sptr<CallInfo> call )
{
	nextEvent->setCallInfo ( callInfo, calls );
	stateMachine->process( nextEvent );

	return;
}

void UaBuilder::setSipStack( const Sptr < SipTransceiver > sipstack )
{
	sipStack = sipstack;
}

void UaBuilder::setSubscribeManager( const Sptr < SubscribeManager > subscribeManager )
{
	subManager = subscribeManager;
}


void UaBuilder::startRegistration()
{
	if ( UaConfiguration::instance()->getRegisterOn() )
	{
		registrationManager = new RegistrationManager( sipStack );
		assert( registrationManager != 0 );
	}
}


/*added by wuaiwu */
void UaBuilder::initCallContainer()
{
	calls.dynamicCast(myCallContainer);
	if (calls == 0)
	{
		cpLog(LOG_EMERG, "No availability UaCalltainer, exit!");
		exit(0);
	}
}

void UaBuilder::initFeature()
{
	stateMachine.dynamicCast( *( myFeatures.begin() ) );
	if (stateMachine == 0)
	{
		cpLog(LOG_EMERG, "No availability UaStateMachine, exit!");
		exit(0);
	}
}
	
