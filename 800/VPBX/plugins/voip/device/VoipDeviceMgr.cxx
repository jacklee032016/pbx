/*
* $Id: VoipDeviceMgr.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "cpLog.h"
#include "PbxAgent.hxx"


#include "CallTimerEvent.hxx"
#include "VoipCtrlDevice.hxx"

#include "VoipEndPoint.hxx"
#include "SipTransceiver.hxx"
#include "SipEvent.hxx"
#include "UaBuilder.hxx"

#include "VoipStateBuilder.hxx"
#include "EpStateMachine.hxx"
#include "VoipDeviceMgr.hxx"

using namespace Assist;


VoipDeviceMgr::VoipDeviceMgr(PbxAgent* agent, bool callLegHistory )
	:DeviceMgr(agent),
	myCallLegHistory(callLegHistory)
{
	EpStateMachine *stateMachine = new EpStateMachine(this);

	stateMachine->addState( new VoipStateIdle );
	stateMachine->addState( new VoipStateRinging);
	stateMachine->addState( new VoipStateTrying);
	stateMachine->addState( new VoipStateCancelTrying);
	stateMachine->addState( new VoipStateInCall);
	
	setPbxStateMachine( stateMachine);

	myUaBuilder = new UaBuilder(this, Data( PbxConfiguration::instance()->getLocalSipPort() ).convertInt() );
	mySipStack = myUaBuilder->getSipStack();
	assert( mySipStack!=0);
	
    	myUaBuilder->startRegistration();
	
	if ( init() )
		assert(0);

	myUaBuilder->run();
}


VoipDeviceMgr::~VoipDeviceMgr()
{
	//destory();
}

void VoipDeviceMgr::mainLoop (void* params)
{
	myMainLoop(params); 
}

void VoipDeviceMgr::myMainLoop(void *params)
{
	try
	{
		assert ( mySipStack != 0 );
//		assert ( myOutputFifo != 0 );

		Sptr <SipMsgQueue> sipRcv( mySipStack->receive(1000) );

		if ( sipRcv != 0 )
		{
			cpLog(LOG_DEBUG, "sipRcv != 0");
			Sptr <SipMsg> sipMsg = sipRcv->back();

			if ( sipMsg != 0 )
			{
				cpLog(LOG_DEBUG, "sipMsg  != 0");
				assert(sipMsg != 0);

				myUaBuilder->processSipMsg( sipMsg, sipRcv);
			}
		}
	}
	catch ( VException& v)
	{
		cpDebug(LOG_DEBUG, "VException from SIP  stack %s", v.getDescription().c_str() );
	}
	catch ( ... )
	{
		cpDebug(LOG_DEBUG, "Exception from SIP stack" );
	}
}

int VoipDeviceMgr::init()
{
	for (int i = 0; i < PbxConfiguration::instance()->getPbxCount(); ++i)
	{
		CtrlDevice *pdev = new VoipCtrlDevice(this, mySipStack, i, "VOIP");
		addDevice(pdev);
	}

	return 0;
}

/* find a free EP, this function is only called by CallOpRoute */
Sptr <EndPoint>  VoipDeviceMgr::lookupEndPointByNumber(const char *extPhoneNumber)
{
#if 0
	int index = 0;
	Sptr <CtrlDevice> ctrl;
	while ((ctrl = getNextDevice(index)) != NULL)
	{
		Sptr <EndPoint> ep = ctrl->getNextEP(NULL);
		if (ep != 0 && ep->getState()->getStateType() == VOIP_EP_STATE_IDLE )
		{
			cpLog(LOG_DEBUG, "Find %s which is idle(destination phone number %s is nou used)", ep->getName(), extPhoneNumber);
			return ep;
		}		
		++index;
	}

	cpLog(LOG_DEBUG, "VoipDeviceMgr::lookupEndPointByNumber NOT found a EP");
	return NULL;
#endif
	return findFreeEP();

}


Sptr <VoipEndPoint> VoipDeviceMgr::getEndPointByPhoneNumber(const char *phone, bool isInvite)
{
	Sptr <VoipEndPoint> vep;
	int index = 0;
	Sptr <CtrlDevice> ctrl =NULL;
	
	if (isInvite)
	{/* find a IDLE ep, and set it's phonenumber as this phone  */
		while ((ctrl = getNextDevice(index)) != NULL)
		{
			Sptr <EndPoint> ep = ctrl->getNextEP(NULL);
			if ( ep->getState()->getStateType() == VOIP_EP_STATE_IDLE )
			{
				ep->setPhoneNumber(phone);
				cpLog(LOG_DEBUG, "Find %s", ep->getName() );
				
				vep.dynamicCast(ep);
				return vep;
			}		
			++index;
		}

		cpLog(LOG_WARNING, "No free VoipEndPoint for this INVITE");
		return NULL;
	}

	index = 0;
	while ((ctrl = getNextDevice(index)) != NULL)
	{/* find a EP with phone number, that EP is in state of not IDLE */
		Sptr <EndPoint> ep = ctrl->getNextEP(NULL);
		if (ep != 0 && strcmp(ep->getPhoneNumber(), phone )== 0)
			//ep->getState()->getStateType() == VOIP_EP_STATE_IDLE )
		{

			if( vep->getState()->getStateType() != VOIP_EP_STATE_IDLE )
			{
				cpLog(LOG_DEBUG, "Find %s with destination phone number %s", ep->getName(), phone);
				vep.dynamicCast(ep);
				return vep;
			}
			cpLog(LOG_ERR, "VoipEndPoint %s is in IDEL state but this msg is not INVITE", ep->getName() );
		}		
		++index;
	}

	return NULL;
}

Sptr <VoipEndPoint> VoipDeviceMgr::getEndPointBySipCallId(SipCallId &sipCallId)
{
	Sptr <VoipEndPoint> vep;
	int index = 0;
	Sptr <CtrlDevice> ctrl =NULL;
	
	while ((ctrl = getNextDevice(index)) != NULL)
	{
		Sptr <EndPoint> ep = ctrl->getNextEP(NULL);
//		if ( ep->getState()->getStateType() == VOIP_EP_STATE_IDLE )
		{
//			ep->setPhoneNumber(phone);
			cpLog(LOG_DEBUG, "Find %s", ep->getName() );
			
			vep.dynamicCast(ep);
			if(vep->getSipCallId() == sipCallId)
				return vep;
		}		
		++index;
	}

	cpLog(LOG_WARNING, "No free VoipEndPoint for this INVITE");
	return NULL;

}

Sptr <EndPoint> VoipDeviceMgr::findFreeEP()
{
	int index = 0;
	Sptr <CtrlDevice> ctrl;
	while ((ctrl = getNextDevice(index)) != NULL)
	{
		Sptr <EndPoint> ep = ctrl->getNextEP(NULL);
		if (ep != 0 && ep->getState()->getStateType() == VOIP_EP_STATE_IDLE )
		{
			cpLog(LOG_DEBUG, "Find %s which is idle", ep->getName());
			return ep;
		}		
		++index;
	}

	cpLog(LOG_DEBUG, "VoipDeviceMgr::lookupEndPointByNumber NOT found a EP");
	return NULL;

}

