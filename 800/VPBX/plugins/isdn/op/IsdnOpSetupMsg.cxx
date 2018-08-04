/*
 * $Id: IsdnOpSetupMsg.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "IsdnOpBuilder.hxx"
#include "PbxAgent.hxx"

using namespace Assist;

const Sptr <PbxState> IsdnOpSetupMsg::process( const Sptr <PbxEvent> event )
{
	if(checkIsdnMsgType(event) != ISDN_TYPE_SETUP)
	{
		return PBX_STATE_CONTINUE;
	}
	
	Sptr <MsgSetup> isdnSetupMsg;
	isdnSetupMsg.dynamicCast( isIsdnMsg(event));
	assert( isdnSetupMsg != 0 );

	Sptr <AudioChannel> bCh;
	IsdnCtrlDevice *ctrl = getIsdnCtrlDevice(event);

	/* get B channel assigned by SETUP msg */
	if(isdnSetupMsg->getChanId() != 0)
	{
		int bchId = isdnSetupMsg->getChanId()->getChanId();
		bCh = ctrl->allocateBChan( bchId );
	}
	else
	{
		bCh = ctrl->allocateAnyBChan();
	}
	
	Sptr <EndPoint> ep = event->getEndPoint();
	if( bCh==0 )
	{/* no B Channel, reply  RELEASE_COMPLETE msg */
		Sptr <IsdnEvent> isdnEvent;
		isdnEvent.dynamicCast( event );
		assert( isdnEvent != 0 );

		Sptr <IeCause> cause = new IeCause(CAUSE_LOCATION_LPN, CAUSE_T_USER_BUSY );
		Sptr <MsgReleaseComplete> relComMsg = new MsgReleaseComplete( cause, ctrl );
		isdnEvent->setIsdnMsg(relComMsg);

		ep->reset();
		return lookupEPState(event, ISDN_EP_STATE_IDLE);
	}

	/* bind AudioChannel and EndPoint */
	ep->setAudio(bCh);
	bCh->setEndPoint( ep);
#if 0	
	/* first bind, then activate with the help of EndPoint */
	bCh->activate();/* enter into activating state */
#endif

	if( ep->getCallId()==0)
	{
		ep->setCallId( new CallId() );
	}
	else
		cpLog(LOG_WARNING, "CallId has exist for ISDN EndPoint %s", ep->getName() );

	/* send a SETUP CallEvent to CallScheduler */
	CallEvent *callEvent = new CallEvent();
// 	callEvent->setCallMsg( IsdnMsgFactory::instance()->isdn2Call( setupMsg, ep) );

	CommonMsg	*callMsg = new _CallSetup();

#if 1
	bool hasCalledPn = false;
	const char *calledPn;
	char phone[64];

	if(isdnSetupMsg->getCalledPN() != 0)
	{/* Called PN is provided : for NT device, called PN is always provided */
		calledPn = (const char *)isdnSetupMsg->getCalledPN()->getPartyNumber();
		
		int dialLen = strlen(calledPn);
		int codeLen = strlen( PbxConfiguration::instance()->getIsdnLocalDialCode()) ;
		
		if( dialLen > codeLen )
		{
			if(strncmp(calledPn, PbxConfiguration::instance()->getIsdnLocalDialCode(), codeLen )==0)
			{
				hasCalledPn = true;
				sprintf(phone, "*%s", calledPn+codeLen);
			}
		}
		
		if(hasCalledPn == false)
		{
			codeLen = strlen( PbxConfiguration::instance()->getIsdnExternalDialCode() );
			if(dialLen > codeLen)
			{
				if(strncmp(calledPn, PbxConfiguration::instance()->getIsdnExternalDialCode(), codeLen) == 0)
				{
					hasCalledPn = true;

					char access[64];
					memcpy(access, calledPn+codeLen, PbxConfiguration::instance()->getAccessLength()-1 );
					access[PbxConfiguration::instance()->getAccessLength()-1] = 0;
					sprintf(phone, "%s#%s",access, calledPn+codeLen+PbxConfiguration::instance()->getAccessLength() -1 );
				}
			}
		}
	}	

	if(hasCalledPn == true)
	{
		callMsg->setTo(phone );
		callMsg->setSignalType(dtmf_signal_out_band);
	}	
	else
#endif				
	{/* TE device : no Called PN is provided : call to Console */
//		callMsg->setTo("4#500");// PbxAgent::instance()->getGateway()->getConsoleAccessCode() );
		callMsg->setTo( PbxAgent::instance()->getGateway()->getConsoleAccessCode() );
		/* start a Console decoding task */
		callMsg->setSignalType(dtmf_signal_in_band);
	}
			
	if(ep->checkExt() )
	{/* I am NT device, so use my extension phone number */
			callMsg->setFrom( ep->getPhoneNumber() );
	}
	else if(isdnSetupMsg->getCallingPN() != 0)
	{/* NT device and calling PN is provided */
		callMsg->setFrom((const char *)isdnSetupMsg->getCallingPN()->getPartyNumber() );
	}
	else
	{/* NT device and calling PN is not provided */
		callMsg->setFrom( ctrl->getParent()->getAccessCode() );
	}

	callEvent->setCallMsg( callMsg);
	callEvent->setCallId( ep->getCallId() );
	callEvent->setEndPoint( ep);

#if 0
	Sptr <MsgAlerting> alerting = new MsgAlerting();
	alerting->sendOut( ep);

	return PBX_STATE_CONTINUE;
#endif

	Sptr <PbxState> newState = lookupEPState(event, ISDN_EP_STATE_IN_SETUP);
	ep->setState(newState );

	sendSchedulerEvent(callEvent);

	if(newState==0)
		cpLog(LOG_DEBUG_STACK, "'%s' state continue after %s", ep->getName(), name() );
	else
		cpLog(LOG_DEBUG_STACK, "'%s' goto state %s after %s", ep->getName(), newState->name(), name() );
	return newState;
}

