/*
 * $Id: IsdnOpSetupCall.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "IsdnOpBuilder.hxx"

using namespace Assist;

/*
* when EP is in IDLE, receive SETUP call event from CallScheduler 
* and send out a CC_NEW_CR msg to get a CallRef for this call from driver
*/
const Sptr <PbxState> IsdnOpSetupCall::process( const Sptr <PbxEvent> event )
{
	if(checkCallMsg(event) != CALL_SETUP )
	{
		return PBX_STATE_CONTINUE;
	}

	Sptr <CommonMsg> commMsg = isCallMsg(event);
	if( commMsg == 0 )
		return PBX_STATE_CONTINUE;

	Sptr <CallEvent> callEvent = isCallEvent(event);
	assert(callEvent != 0);
	
	cpLog(LOG_DEBUG_STACK, "'%s' is handled in %s", commMsg->_Describer(), name() );
	Sptr <EndPoint> ep = event->getEndPoint();
	ep->setPeerSignalType(commMsg->getPeerSignalType() );
	
	Sptr <IsdnEndPoint> isdnEp = NULL;
	isdnEp.dynamicCast(ep);
	assert(isdnEp != 0);

	Sptr <DeviceMgr> mgr = ep->getCtrlDevice()->getParent();
	Sptr <IsdnDeviceMgr> isdnMgr = NULL;
	isdnMgr.dynamicCast( mgr);
	assert(isdnMgr != 0);

	/* allocate B channel for this EndPoint */
	IsdnCtrlDevice *isdnCtrl = getIsdnCtrlDevice(event);
	Sptr <AudioChannel> audioCh = isdnCtrl->allocateAnyBChan();

	if( audioCh == NULL)
	{/* no B Channel is available */
		CallEvent *callEvent = new CallEvent();
		_CallRelease *rel = new _CallRelease();
		rel->setCause( CAUSE_T_USER_BUSY);
	 	callEvent->setCallMsg( rel );
		callEvent->setCallId( ep->getCallId() );
		callEvent->setEndPoint( ep);

		sendSchedulerEvent(callEvent);

		ep->reset();
		return ep->findState(ISDN_EP_STATE_IDLE);
	}
	
	Sptr <IsdnAudioChannel> bCh = NULL;
	bCh.dynamicCast( audioCh);
	assert(bCh!=0);

	/* bind together */
	ep->setAudio( bCh);
	bCh->setEndPoint( ep);
#if 0	
	/* bind must before activate */
	audioCh->activate();/* enter into activating state */
#endif
	/* 1: send a dinfo into kernel */	
	isdnEp->setFrameInfo( isdnMgr->newFrameInfo() );
	Sptr <MsgCCNew> newCrMsg = new MsgCCNew( isdnCtrl );
	newCrMsg->sendOut( ep);

	/* store call for send out SETUP msg */
//	ep->setCallMsg(commMsg);

	/* 2 : send out a SETUP */
	Sptr <MsgSetup> setup = new MsgSetup( isdnCtrl );
	IeChanId *chanId = new IeChanId(CHAN_ID_MODE_EXCLUSIVE, bCh->getChanId() );
	IeBearer *bearer = new IeBearer();

#if 0
	if(isdnCtrl->getNtMode() == ISDN_DEV_TYPE_USER)
	{	
		IeCalledPN  *called = new IeCalledPN();
		called->setPartyNumber( (unsigned char *)commMsg->getTo() );
//		called->setPartyNumber( (unsigned char *)commMsg->getFrom() );
		setup->setCalledPN( called );
	}	
#endif

	if(strlen( commMsg->getTo()) != 0)
	{
		cpLog(LOG_DEBUG, "ISDN CalledPN in SETUP is %s", commMsg->getTo() );

		IeCalledPN *callee = new IeCalledPN();
		if(ep->checkExt() )
		{/* NT : extension phone number, send to TA */
			char num[10];
			sprintf(num, "00%d", bCh->getChanId() );
			callee->setPartyNumber((unsigned char *)num);
		}
		else
		{/* TE : external phone number */
			string peerNumber;
			peerNumber = commMsg->getTo();
			string::size_type pos = peerNumber.find("#");
			cpDebug(LOG_DEBUG, "# is in poistion %d of %s" , pos, peerNumber.c_str() );
			if (pos != string::npos && pos< string::npos )
			{
				cpDebug(LOG_DEBUG, "# is in poistion %d of %s" , pos, peerNumber.c_str() );
				peerNumber= peerNumber.substr(pos+1, peerNumber.length()-pos-1);
			}
			callee->setPartyNumber((unsigned char *)peerNumber.c_str() );
		}
		
//		callee->setPartyNumber((unsigned char *)commMsg->getTo() );
//		callee->setPartyNumber((unsigned char *)"02" );
		setup->setCalledPN( callee);
	}
	
	if(strlen( commMsg->getFrom()) != 0)
	{
		cpLog(LOG_DEBUG, "ISDN CallingPN in SETUP is %s", commMsg->getFrom() );
		IeCallingPN *calling = new IeCallingPN();
		calling->setPartyNumber((unsigned char *)commMsg->getFrom() );
		setup->setCallingPN( calling);
	}
	
	setup->setChanId( chanId);
	setup->setBearer( bearer);
	setup->sendOut( ep);

	return lookupEPState(event, ISDN_EP_STATE_OUT_SETUP) ;
}

