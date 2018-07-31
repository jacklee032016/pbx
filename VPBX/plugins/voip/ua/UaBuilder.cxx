/*
* $Id: UaBuilder.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"
#include "StatusMsg.hxx"
#include "InviteMsg.hxx"
#include "SipCommand.hxx"
#include "SipTransceiver.hxx"
#include "SipTransceiverFilter.hxx"
#include "SubscribeThread.hxx"

#include "voip_sip.hxx"
#include "SipAllow.hxx"

#include "SipContext.hxx"

#include "SipEvent.hxx"

#include "VoipEndPoint.hxx"
#include "VoipDeviceMgr.hxx"
#include "PbxConfiguration.hxx"

#include "UaBuilder.hxx"
using namespace Assist;
#if 0
bool discardMessage( Sptr <SipMsg> &sipMsg) const
{
	int numberOfVia = sipMsg->getNumVia();

	// check required only if > 2 via's in msg
	if ( numberOfVia <= 2 )
	{
		return false;
	}

	SipVia mySipVia;
	mySipVia.setHost(theSystem.gethostAddress());

	for ( int i=0, loopCount = 0; i < numberOfVia; i++ )
	{
		const SipVia& recvSipVia = sipMsg->getVia(i);

		if ( recvSipVia == mySipVia )
		{
			loopCount++;
			if ( loopCount > 2 )
			{
				cpDebug(LOG_DEBUG, "Discarding Msg, loop > %d. %s", loopCount, recvSipVia.encode().logData());
				return ( true );
			}
		}
	}

	return ( false );
}
#endif


Sptr <RegistrationManager> UaBuilder::myRegistrationManager = 0;

 UaBuilder::UaBuilder(VoipDeviceMgr *deviceMgr, unsigned short defaultSipPort , Data applName, bool  filteron, bool  nat ,  SipAppContext aContext )
 	:myDeviceMgr(deviceMgr)
{
	if ( filteron= true )
	{
		mySipStack = new SipTransceiverFilter(applName, defaultSipPort, nat, aContext);
	}
	else
	{
		mySipStack = new SipTransceiver(applName, defaultSipPort, nat, aContext);
	}

#if	__CDR_OPEN__
	if (PbxConfiguration::instance()->getCdrOn())
	{
		cpDebug(LOG_DEBUG, "Create RecordThread in that time." );
		myRecordThread = new RecordThread;        
		assert (myRecordThread != 0);
	}
	else 
	{
		cpDebug(LOG_DEBUG, "CDR service not started!" );
		myRecordThread = 0;
	}
#endif

	cpDebug(LOG_DEBUG, "Create SubscribeManager" );
	mySubscribeManager = new SubscribeManager( mySipStack );

	if ( PbxConfiguration::instance()->getSubscribeOn() )
	{
		cpDebug(LOG_DEBUG, "Create SubScribe Thread" );
		mySubscribeThread = new SubscribeThread( mySubscribeManager);
		assert( mySubscribeThread != 0 );
	}
	
} 

UaBuilder::~UaBuilder()
{} 

void UaBuilder::run()
{
	if ( mySubscribeThread != 0 )
	{
		cpDebug(LOG_DEBUG, "Spawning Feature Thread" );
		mySubscribeThread->run();
	}
	
#if __CDR_OPEN__
	if(myRecordThread != 0)
	{
		cpDebug(LOG_DEBUG, "Spawning Record Thread");
		myRecordThread->run();
	}
#endif

#if 0
	if ( loadGenThread != 0 )
	{
		cpDebug(LOG_DEBUG, "Spawning Load Generation Monitor Thread");
		loadGenThread->run();
	}

	// User TimerEvent to kick start the load generator
	if ( PbxConfiguration::instance()->getLoadGenOn() )
	{
		if (PbxConfiguration::instance()->getRunMode() == "Calling" ||
			PbxConfiguration::instance()->getRunMode() == "RSTest" )
		{
			printf("*****  Create kick start event *****\n");
			int numStarts = PbxConfiguration::instance()->getNumKickStarts();
			if (numStarts < 1)
			{
				numStarts = 1;
			}

			int avgCallTime = 500;  // average length of one call is 500 ms
			int delay = avgCallTime / numStarts;  //kickstart all units in 500 ms
			for (int i = 0; i < numStarts; i++)
			{
				++numOfCallsStarted;
				Sptr < UaDigitTimerEvent > kickStartEvent = new UaDigitTimerEvent( myCallProcessingQueue );
				assert (kickStartEvent != 0);

				kickStartEvent->setLoadGenSignalType(LoadGenStartCall);
				kickStartEvent->setSipStack( mySipStack );

				myCallProcessingQueue->addDelayMs( kickStartEvent, (PbxConfiguration::instance()->getStartTime()) +(delay* i));
			}
		}
	}
#endif	

}

/* play the role as STATE BUSY_HERE : Ack(AckMsg) will be reply for this BUSY_HERE status msg?? */
void UaBuilder::replyBusyHere(Sptr <InviteMsg> inviteMsg)
{
	if ( inviteMsg == 0 )
	{
		return;
	}
    /* modify by wuaiwu */
#if 0
    Sptr < SipContext > call;
    call.dynamicCast( event->getCallInfo() );
    assert( call != 0 );
     if(!call->getTimerSet())
     {
	 setTimer(event, 10); 
	 call->setTimerSet(true);
     }
#endif 
    /*end of modify */

	cpDebug(LOG_INFO, "Busy - Cannot accept call: %s" ,inviteMsg->getFrom().encode().logData());

	StatusMsg status( *inviteMsg, SIP_STATUS_CODE_CERROR_BUSY_HERE );
    // Add myself to the contact
	Sptr <SipUrl> myUrl = new SipUrl;
	myUrl->setUserValue( PbxConfiguration::instance()->getUserName(), "phone" );
	myUrl->setHost(inviteMsg->getRequestLine().getHost());
	myUrl->setPort( PbxConfiguration::instance()->getLocalSipPort() );

	SipContact me;
	me.setUrl( myUrl );
	
	status.setNumContact( 0 );    // Clear
	status.setContact( me );

	mySipStack->sendReply(status); //, hosts ,  port);

}


/* A child 'handle' method */
bool UaBuilder::handleStatusMsg( const Sptr <SipMsg> msg )
{
	cpDebug(LOG_DEBUG, "In handleStatusMsg()" );

	Sptr <StatusMsg> statusMsg;
	statusMsg.dynamicCast(msg);
	/// Register
	int RegCode = SIP_STATUS_CODE_SUCCESS_OK;
	if ( PbxConfiguration::instance()->getRegisterOn() && statusMsg->getType() == SIP_REGISTER/* getCSeq().getMethod() == "REGISTER" */)
	{
		if ( statusMsg->getStatusLine().getStatusCode() < SIP_STATUS_CODE_SUCCESS_OK )
		{
			/// just return, prevents re-registration loop
			cpDebug(LOG_DEBUG, "REGISTER response: code %d", statusMsg->getStatusLine().getStatusCode() );
			return true;
		}

		string num = statusMsg->getTo().getUser().convertString();

		cpLog(LOG_DEBUG, "num = %s", num.c_str());

		Sptr <VoipEndPoint> ep = myDeviceMgr->getEndPointByPhoneNumber( num.c_str(), false);
		
		if (ep == 0)
		{
			cpLog(LOG_WARNING, "No VOIP EndPoint with phone number of %s is found", num.c_str() );
			return true;
		}	

		RegCode =  myRegistrationManager->handleRegistrationResponse( *statusMsg );
		cout << "RegCode = " << RegCode << endl;
		if ( RegCode == SIP_STATUS_CODE_SUCCESS_OK)
		{
			//cout << VTime::strftime("%y/%m/%d %H:%M:%S")
			cout  << " Registration OK" << endl;
			cout << "Ready" << endl;
			ep->reportDeviceInfo(ACTION_UNKNOW, "Regist OK");
		}
		else if (RegCode == SIP_STATUS_CODE_CERROR_FORBIDDEN)
		{
			// cpLog( LOG_WARNING, "Registration status code %d", statusMsg->getStatusLine().getStatusCode() );
			cout << "Registration forbidden" << endl;
			ep->reportDeviceInfo(ACTION_UNKNOW, "Regist Forbidden");
		}
		return true;
	}

	/// Subscribe
	if ( PbxConfiguration::instance()->getSubscribeOn() && statusMsg->getType() == SIP_SUBSCRIBE /*->getCSeq().getMethod() == "SUBSCRIBE"*/ )
	{
		if ( mySubscribeManager == 0 )
		{
			cpLog( LOG_ERR, "SubscribeManager is not running" );
		}
		else
		{
			if ( mySubscribeManager->handleSubscribeResponse( *statusMsg ) )
				cpLog( LOG_ERR, "Subscribe Ok" ); //cout << "Subscribe Ok" << endl;
			else
				cpLog( LOG_ERR, "Subscribe failed" );
		}
		return true;
	}

	/// Notify
	if ( PbxConfiguration::instance()->getSubscribeOn()&& statusMsg->getType() == SIP_NOTIFY )
	{
		Sptr <NotifyMsg> notifyMsg;
		notifyMsg.dynamicCast(msg );
		assert(notifyMsg != 0);
//		if ( notifyMsg != 0 )
		{
			mySubscribeManager->handleNotify( *notifyMsg );
			return true;
		}
		
	}

	return false;
}

bool UaBuilder::handleCallWaiting( const Sptr <InviteMsg> sipMsg )
{
#if 0
	int deviceId = sipEvent->getDeviceId();

	Sptr <SipCallId> newCallId = new SipCallId(sipMsg->getCallId() );

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

		Sptr <SipCallId> activeId = UaDevice::instance()->getCallId( deviceId );

		/// allow call waiting only if the current call is talking to one party
		if ( activeId != 0 )
		{
			Sptr < SipContext > activeCall = calls->findCall( *activeId ,deviceId );
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
#endif

	return true;
}

bool UaBuilder::processSipMsg( const Sptr <SipMsg> sipMsg, Sptr <SipMsgQueue> sipRcv )
{
	Sptr <VoipEndPoint> ep;
	cpDebug(LOG_DEBUG, "SIP message event In processSipMsg" );

	Sptr <StatusMsg> statusMsg;
	statusMsg.dynamicCast( sipMsg);

	if ( statusMsg != 0 )
	{
		if( handleStatusMsg( statusMsg) )
		{
			return true;
		}
	}

	int msgType = sipMsg->getType();
	if ( (msgType != SIP_INVITE ) &&  msgType != SIP_ACK && msgType != SIP_CANCEL && msgType != SIP_BYE && statusMsg== 0)
	{
		Sptr <SipCommand>  scomm;
		scomm.dynamicCast(sipMsg );
		if (scomm != 0)
		{
			StatusMsg sallow(*scomm, SIP_STATUS_CODE_CERROR_METHOD_NOT_ALLOW);
			SipAllow allow("INVITE,ACK,CANCEL,BYE");
			sallow.setAllow(allow);
			
			mySipStack->sendReply(sallow);
		}
		
		return true;
	}

#if 0	
	/* added by lizhijie ,2004.02.12, 02.17 */
	Sptr <SipCallLeg> sipCallLeg;
	sipCallLeg = sipEvent->getSipCallLeg();
	callId = sipCallLeg->getCallId();
#else
	SipCallId sipCallId = sipMsg->getCallId();
#endif
//	Sptr <SipCallId> newCallId = new SipCallId(callId);

	string calleenum, callernum, num;

	if(msgType == SIP_BYE )///*"BYE"*/ ||statusMsg !=0 )
	{
		if(statusMsg == 0)
		{
			num = sipMsg->getTo().getUser().convertString();
		}	
		else
		{/* this status msg is reply for msg send by my EP */
			num = sipMsg->getFrom().getUser().convertString();
		}	
				
		cpLog(LOG_ERR, "process BYE msg, %s", num.c_str());
//		ep = myDeviceMgr->getEndPointByPhoneNumber(num.c_str(),false);
		ep = myDeviceMgr->getEndPointBySipCallId(sipCallId);
		if( ep == 0)
		{
			cpLog(LOG_ERR, "Can't find the VOIP EndPoint with the SipCallId %s, ignore this BYE msg", sipCallId.encode().logData() );
			return true;
		}

		ep->reportSipEvent( sipMsg,sipRcv);
		return false;
	}
	else if(statusMsg !=0)
	{
		callernum = sipMsg->getFrom().getUser().convertString();
//		ep = myDeviceMgr->getEndPointByPhoneNumber( callernum.c_str(), false);
		ep = myDeviceMgr->getEndPointBySipCallId(sipCallId);
#if 0
		if(deviceId != -1 )
		{
		}
		else
		{
			cpDebug(LOG_DEBUG, "----------------------------");
			sipEvent->setDeviceId(AS_DEVICE_NOT_DEFINED);
			callInfo = calls->newCall( callId , AS_DEVICE_NOT_DEFINED);
			callInfo->setFeature( stateMachine );
			callInfo->setState( stateMachine->findState( "StateBusyHere" ) );
			return false;
		}

#endif 
		if ( ep == 0)
		{
			cpLog(LOG_ERR, "Can't find a VOIP EndPoint with SipCallId %s, ignore this status \n%s",  sipCallId.encode().logData() , statusMsg->encode().logData());
//			assert(0);
			return true;
		}
		cpLog(LOG_ERR, "report STATUS msg to statemachine \n%s", statusMsg->encode().logData() );

		ep->reportSipEvent( sipMsg,sipRcv);
		return false;
	}
	else if( msgType == SIP_ACK)
	{/* for the ACK msg reply for out BUSY_HERE */
		if(statusMsg == 0)
		{
			num = sipMsg->getTo().getUser().convertString();
		}	
		else
		{/* this status reply for msg send by my EP */
			num = sipMsg->getFrom().getUser().convertString();
		}	
				
		cpLog(LOG_ERR, "process ACK msg \n%s", sipMsg->encode().logData());
//		ep = myDeviceMgr->getEndPointByPhoneNumber(num.c_str(),false);
		ep = myDeviceMgr->getEndPointBySipCallId(sipCallId);
		if( ep == 0)
		{
			cpLog(LOG_ERR, "Can't find the VOIP EndPoint with SipCallId %s, ignore this ACK msg", sipCallId.encode().logData());
			return true;
		}

		cpLog(LOG_ERR, "report ACK msg to statemachine \n%s", sipMsg->encode().logData() );
		ep->reportSipEvent( sipMsg,sipRcv);

		return true;
	}

	else if( msgType == SIP_CANCEL )
	{/* for the ACK msg reply for out BUSY_HERE */
		if(statusMsg == 0)
		{
			num = sipMsg->getTo().getUser().convertString();
		}	
		else
		{/* this status reply for msg send by my EP */
			num = sipMsg->getFrom().getUser().convertString();
		}	
				
		cpLog(LOG_ERR, "process CANCEL msg \n%s", sipMsg->encode().logData());
//		ep = myDeviceMgr->getEndPointByPhoneNumber(num.c_str(),false);
		ep = myDeviceMgr->getEndPointBySipCallId(sipCallId);
		if( ep == 0)
		{
			cpLog(LOG_ERR, "Can't find the VOIP EndPoint with SipCallId %s, ignore this CANCEL msg", sipCallId.encode().logData());
			return true;
		}

		cpLog(LOG_ERR, "report CANCEL msg to statemachine \n%s", sipMsg->encode().logData() );
		ep->reportSipEvent( sipMsg,sipRcv);

		return true;
	}


#if 0
	//when the callinfo's statemachine is StateBusyHere
	callInfo = calls->findCall( callId, AS_DEVICE_NOT_DEFINED);
	if(callInfo != 0)
	{
		sipEvent->setDeviceId(AS_DEVICE_NOT_DEFINED);
		return false;
	}
#endif

	Sptr <InviteMsg> invitemsg;
	invitemsg.dynamicCast( sipMsg);
//	assert(invitemsg != 0);
	
	if(sipMsg->getTo().getUser() == sipMsg->getFrom().getUser() )
	{
		// dialed to itself
		if(invitemsg!=0)
		{
			cpDebug(LOG_DEBUG, "Busy here for new call to device itself");
			replyBusyHere( invitemsg);

			return false;
		}	

		return true;
	}
	
//	bool isInvite = (invitemsg != 0)? true : false;
	calleenum = sipMsg->getTo().getUser().convertString();
	
//	ep = myDeviceMgr->getEndPointByPhoneNumber( calleenum.c_str(), isInvite);

	cpLog(LOG_ERR, "process INVITE msg " );
	ep.dynamicCast( myDeviceMgr->findFreeEP() );
	if(ep != 0)
	{
		cpLog(LOG_DEBUG, "Find a free EndPoint %s for phone number %s", ep->getName(), num.c_str());

		ep->reportSipEvent(sipMsg ,sipRcv);
	}
	else
	{/* no EP for this calleenumber of SipMsg */	
		cpDebug(LOG_DEBUG, "No Voip EndPoint is for SipCallId %s in InviteMsg", sipCallId.encode().logData() );
		assert(invitemsg != 0);
		replyBusyHere( invitemsg);
		return false;
	}

	return true;
}  

const Sptr <SipTransceiver> UaBuilder::getSipStack( )
{
	return mySipStack;
}

void UaBuilder::startRegistration()
{
	if ( PbxConfiguration::instance()->getRegisterOn() )
	{
		myRegistrationManager = new RegistrationManager( mySipStack );
		assert( myRegistrationManager != 0 );
	}
}


