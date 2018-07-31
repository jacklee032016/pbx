/*
* $Id: IsdnDeviceMgr.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"
#include "cpLog.h"
#include "vpbx_globals.h"
#include "PbxAgent.hxx"

#include "CallTimerEvent.hxx"

#include "IsdnTeDevice.hxx"
#include "IsdnNtDevice.hxx"
#include "IsdnMgrThread.hxx"

#include "EpStateMachine.hxx"

#include "IsdnStateBuilder.hxx"

#include "IsdnDeviceMgr.hxx"

using namespace Assist;

IsdnDeviceMgr::IsdnDeviceMgr(PbxAgent *agent )
	:DeviceMgr(agent )
{
	mySupportSecondDial = false;
	myDialMethodType = ISDN_DIAL;
	
	EpStateMachine *stateMachine = new EpStateMachine(this);

	stateMachine->addState( new IsdnStateIdle );
	stateMachine->addState( new IsdnStateSetupIn );
	stateMachine->addState( new IsdnStateSetupOut );
	stateMachine->addState( new IsdnStateProceedingIn );
	stateMachine->addState( new IsdnStateProceedingOut );
	stateMachine->addState( new IsdnStateAlertingIn );
	stateMachine->addState( new IsdnStateAlertingOut );
	stateMachine->addState( new IsdnStateDisconnectIn );
	stateMachine->addState( new IsdnStateDisconnectOut );
	stateMachine->addState( new IsdnStateConnect );
	stateMachine->addState( new IsdnStateWaitClearCR );
	stateMachine->addState( new IsdnStateError );
#if 0
	stateMachine->addState( new EpStateOverlapIn );
	stateMachine->addState( new EpStateOverlapOut );
#endif
	
	setPbxStateMachine( stateMachine);
	
	myDebugOption = ASNT_DEBUG_OPTION;
	
	debug_init( myDebugOption, NULL, NULL, NULL);

#if 0
	/* signal handlers */	
	signal(SIGINT,	sysSignalHandler);
	signal(SIGHUP,	sysSignalHandler);
	signal(SIGTERM,	sysSignalHandler);
	signal(SIGPIPE,	sysSignalHandler);
#endif	

	as_msg_init();

	if ( init() )
		assert(0);

	myRandom = new Random();
	myRandom->init();

	myMgrThread = new IsdnMgrThread(this);
}


IsdnDeviceMgr::~IsdnDeviceMgr()
{
	cpLog(LOG_DEBUG, "Destorying.....");
	destory();

	debug_close();
}

void IsdnDeviceMgr:: mainLoop( void* parms )
{
	myMainLoop( parms);
}

/* all Layer2 message is handler in this function by DeviceThread directly */
void IsdnDeviceMgr::myMainLoop(void *params)
{
	int ret = 1; /* handled */
	msg_t *msg;

	if (!(msg = as_msg_alloc(MAX_MSG_SIZE)))
		return;

	cpLog(LOG_DEBUG, "waiting msg from hardware....." );
	ret = ask_isdn_read_without_lock( myFd, msg->data, MAX_MSG_SIZE,TIMEOUT_INFINIT /*TIMEOUT_10SEC*/);
//	ret = ask_isdn_read_without_lock( myFd, msg->data, MAX_MSG_SIZE, 0);
//	ret = read( myFd, msg->data, MAX_MSG_SIZE );
	cpLog(LOG_DEBUG, " RX!!" );
	if (ret < 0)
	{
		if (errno == EAGAIN)
			return ;
		cpLog(LOG_ERR, "FATAL ERROR: failed to do ask_isdn_read(%d: %s)\n", ret, strerror(errno) );
		as_msg_free(msg);
		msg=0;
		cpLog(LOG_ERR, "FATAL ERROR: failed to do ask_isdn_read()\n");
		exit(-1); 
	}
	if (!ret)
	{
		as_msg_free(msg);
		msg=0;
		return;
	}
	if(ret > MAX_MSG_SIZE)
	{
		cpLog(LOG_ERR, "overflow for ISDN msg");
		assert(0);
	}
	msg->len = ret;

	ask_hex_dump(msg->data, msg->len);
	reportMsgEvent(msg, NULL);
	
	return;	
}

Sptr <CtrlDevice> IsdnDeviceMgr::getDeviceByStackId( int  stackId )
{
	Sptr <CtrlDevice> dev = NULL;

	while ( (dev = getNextDevice(dev)) != 0 )
	{
		Sptr <IsdnCtrlDevice> isdnCtrl;
		isdnCtrl.dynamicCast(dev);
		assert(isdnCtrl != 0);
		
		if ( isdnCtrl->getStackId() == stackId )
		{
			return dev;
		}
	}

	return NULL;	
}

Sptr <CtrlDevice> IsdnDeviceMgr::getDeviceByAddress(unsigned int address )
{
	Sptr <CtrlDevice> dev = NULL;

	while ( (dev = getNextDevice(dev)) != 0  )
	{
		Sptr <IsdnCtrlDevice> isdnCtrl;
		isdnCtrl.dynamicCast(dev);
		assert(isdnCtrl != 0);
		
		if ( isdnCtrl->getAddress() == address )
		{
			return dev;
		}
#if 0		
		if( isdnCtrl->lookupAudioCh( address) != NULL)
			return dev;
#endif		
	}

	return NULL;	
}
	
CtrlDevice *IsdnDeviceMgr::createDevice(int nt , int _portnum, int _ptp )
{
	assert(_portnum>0 && _portnum<5);
	CtrlDevice *_dev;
	
	if(nt)
	{
		_dev= new  IsdnNtDevice(this, myFd, _portnum,  _ptp );
	}
	else
	{
		_dev = new IsdnTeDevice(this, myFd, _portnum,  _ptp);
	}
	
	return _dev;
}

/* global function to show all available isdn ports */
void IsdnDeviceMgr::devsInfo(void)
{
	int err;
	int i, ii, p;
	int useable, nt = 0;
	unsigned char buff[1025];
	iframe_t *frm = (iframe_t *)buff;
	stack_info_t *stinf;

	/* get number of stacks */
	i = 1;
	ii = ask_isdn_get_stack_count( myFd);
	cpLog( LOG_DEBUG, "\n");
	if (ii <= 0)
	{
		cpLog(LOG_ERR,"Found no card. Please be sure to load card drivers.\n");
	}

	/* loop the number of cards and get their info */
	while(i <= ii)
	{
		err = ask_isdn_get_stack_info(myFd, i, buff, sizeof(buff));
		if (err <= 0)
		{
			cpLog(LOG_ERR, "ask_isdn_get_stack_info() failed: port=%d err=%d\n", i, err);
			break;
		}
		stinf = (stack_info_t *)&frm->data.p;

		useable = 1;

		/* output the port info */
		cpLog( LOG_DEBUG, "Port %2d: ", i);
		switch(stinf->pid.protocol[0] & ~ISDN_PID_FEATURE_MASK)
		{
			case ISDN_PID_L0_TE_S0:
				cpLog( LOG_DEBUG,"TE-mode BRI S/T interface line (for phone lines)");
				nt = 0;
				break;
				
			case ISDN_PID_L0_NT_S0:
				nt = 1;
				cpLog( LOG_DEBUG,"NT-mode BRI S/T interface port (for phones)");
				break;
			default:
				useable = 0;
				cpLog( LOG_DEBUG,"unknown type 0x%08x",stinf->pid.protocol[0]);
		}
		cpLog( LOG_DEBUG,"\n");

		if (nt)
		{
			if (stinf->pid.protocol[1] == 0)
			{
				useable = 0;
				cpLog( LOG_DEBUG," -> Missing layer 1 NT-mode protocol.\n");
			}
			p = 3;

			while(p <= MAX_LAYER_NR)
			{
				if (stinf->pid.protocol[p])
				{
					useable = 0;
					cpLog( LOG_DEBUG," -> Layer %d protocol 0x%08x is detected, but not allowed for NT lib.\n", p, stinf->pid.protocol[p]);
				}
				p++;
			}

			if (useable)
			{
				cpLog( LOG_DEBUG," -> Interface can be Poin-To-Point/Multipoint.\n");
			}
		}
		else
		{
			if (stinf->pid.protocol[1] == 0)
			{
				useable = 0;
				cpLog( LOG_DEBUG," -> Missing layer 1 protocol.\n");
			}
			if (stinf->pid.protocol[2] == 0)
			{
				useable = 0;
				cpLog( LOG_DEBUG," -> Missing layer 2 protocol.\n");
			}
			if (stinf->pid.protocol[2] & ISDN_PID_L2_DF_PTP)
			{
				cpLog( LOG_DEBUG," -> Interface is Poin-To-Point.\n");
			}
			if (stinf->pid.protocol[3] == 0)
			{
				useable = 0;
				cpLog( LOG_DEBUG," -> Missing layer 3 protocol.\n");
			}
			else
			{
				cpLog( LOG_DEBUG," -> Protocol: ");
				switch(stinf->pid.protocol[3] & ~ISDN_PID_FEATURE_MASK)
				{
					case ISDN_PID_L3_DSS1USER:
						cpLog( LOG_DEBUG,"DSS1 (Euro ISDN)");
						break;


					case ISDN_PID_L3_INS_USER:
						cpLog( LOG_DEBUG,"INS User(Japan ISDN)");
						break;
						
					default:
						useable = 0;
						cpLog( LOG_DEBUG,"unknown protocol 0x%08x",stinf->pid.protocol[3]);
				}
				cpLog( LOG_DEBUG,"\n");
			}
			
			p = 4;
			while(p <= MAX_LAYER_NR)
			{
				if (stinf->pid.protocol[p])
				{
					useable = 0;
					cpLog( LOG_DEBUG," -> Layer %d protocol 0x%08x is detected, but not allowed for TE lib.\n", p, stinf->pid.protocol[p]);
				}
				p++;
			}
		}

		if (!useable)
			cpLog( LOG_DEBUG," * Port NOT useable for PBX\n");

		cpLog( LOG_DEBUG,"--------\n");

		i++;
	}
	cpLog( LOG_DEBUG,"\n");

}


int IsdnDeviceMgr::init()
{
	int ret;
	unsigned char buff[1025];
	iframe_t *frm = (iframe_t *)buff;
	stack_info_t *stack_info;
	int i, cnt;
	int nt = 0;
	CtrlDevice *ctrl;

	myFd = ask_isdn_open();
	assert ( myFd > 0);
	cpLog(LOG_INFO, "mainFd =%d" , myFd);

	devsInfo( );

	/* create entity for layer 3 TE-mode */
	ask_isdn_write_frame(myFd, buff, 0, MGR_NEWENTITY | REQUEST, 0, 0, NULL, TIMEOUT_1SEC);
	ret = ask_isdn_read_frame(myFd, frm, sizeof(iframe_t), 0, MGR_NEWENTITY | CONFIRM, TIMEOUT_1SEC);
	assert (ret >= (int)AS_ISDN_HEADER_LEN);
		
	myEntity = frm->dinfo & 0xffff;
	assert (myEntity);
	
	cpLog(LOG_DEBUG_STACK,  "Entity for TE l3-processes is %d.\n", myEntity);

	/* query port's requirements */
	cnt = ask_isdn_get_stack_count(myFd);
	assert (cnt > 0);

//	for(i=1;i <= ISDN_PORT_COUNT ; i++)
	for(i=1;i <= cnt ; i++)
	{
		ret = ask_isdn_get_stack_info(myFd , i, buff, sizeof(buff));
		if (ret < 0)
		{
			cpLog(LOG_ERR, "Cannot get stack info for port %d (ret=%d)\n", i, ret);
			assert(0);;
		}
		
		stack_info = (stack_info_t *)&frm->data.p;
		switch(stack_info->pid.protocol[0] & ~ISDN_PID_FEATURE_MASK)
		{
			case ISDN_PID_L0_TE_S0:
				cpLog(LOG_DEBUG,  "TE-mode BRI S/T interface line\n");
				break;
				
			case ISDN_PID_L0_NT_S0:
				/* added L1 check : ISDN_PID_L1_NT_S0 */
				cpLog(LOG_DEBUG_STACK,  "NT-mode BRI S/T interface port\n");
				nt = 1;
				break;
				
			default:
				cpLog(LOG_ERR, "unknown port %d type 0x%08x\n", i, stack_info->pid.protocol[0]);
				assert(0);
		}

		/* CtrlDevice Only managed in DeviceMgr */
		 ctrl = createDevice(nt, i, 0/*FLAG_PORT_PTP*/);

		 assert( ctrl != 0 );
		 addDevice( ctrl);

		 cpLog(LOG_DEBUG, "run\n");

	}
//	myDeviceCount = i;
	setDeviceCount(i);
	return 0;
}


/* global function to free ALL cards (ports) */
void IsdnDeviceMgr::destory(void)
{
	unsigned char buff[1025];

	cpLog(LOG_DEBUG, "Close ISDN %d port devices......\n", getDeviceCount() );

/*
	int i;
	for(i =0; i <ISDN_PORT_COUNT;i++)
	{
		cpLog(LOG_DEBUG, "Close port devices %s......\n", devs[i]->getName() );
//		delete devs[i];
		devs[i] = NULL;
	}
*/
	
	/* close isdn_fd */
	if (myFd >= 0)
	{
		/* free entity */
		ask_isdn_write_frame(myFd, buff, 0, MGR_DELENTITY | REQUEST, myEntity, 0, NULL, TIMEOUT_1SEC);
		/* close device */
		ask_isdn_close(myFd);
		myFd = -1;
		cpLog(LOG_DEBUG_STACK,  "ISDN device closed.\n");
	}

}

/// Process a msg received from an session
void IsdnDeviceMgr::myProcessHwEvent( Sptr <PbxHwEvent > event )
{
	cpLog(LOG_DEBUG, "Write msg to hardware device");

	return;
}

/* called by callScheduler to routing a call 
* First, check the EXT phone number to get a NT device
* Second, TE device resolve the phone number as dialing plan
*/		
Sptr <EndPoint> IsdnDeviceMgr::lookupEndPointByNumber(const char *extPhoneNumber)
{
	Sptr <CtrlDevice> ctrl = NULL;
	while( (ctrl=getNextDevice( ctrl)) != 0)
	{
		Sptr <IsdnCtrlDevice> isdnCtrl = NULL;
		isdnCtrl.dynamicCast(ctrl);
		assert(isdnCtrl!=0);

		if(isdnCtrl->getNtMode() == 0)
		{/* only TE device is check for external call */
			cpLog(LOG_DEBUG, "Device %s with phone number", ctrl->getName() );
			Sptr <EndPoint> ep = isdnCtrl->lookupFreeEP();
			if( ep != 0)
			{
				return ep;
			}	
		}
	}

	return 0;
}

		/* used in CC_NEW_CR_REQ to get a callRef from driver */
int IsdnDeviceMgr::newFrameInfo()
{
#if 1
	static int dInfo = 0x3000;
#else
	int dInfo ;//= 0x3000;

	Sptr <CtrlDevice> ctrl = NULL;
	Sptr <EndPoint> ep = NULL;

	while( 1)
	{
		while( (ctrl=getNextDevice(ctrl) ) != NULL)
		{
			Sptr <IsdnCtrlDevice> isdnCtrl = NULL;
			isdnCtrl.dynamicCast(ctrl );
			assert(isdnCtrl != 0);
			
			ep = isdnCtrl->lookupEPByFrame( dInfo);
			if(ep != 0)
			{
				dInfo = myRandom->get();
				break;
			}	
		}
		if(ep == 0)
			break;
	}
#endif	
	return dInfo++;
}

int IsdnDeviceMgr::startup()
{
	DeviceMgr::startup();
	
	myMgrThread->run();
	return 0;
}

/*
* RX msg for both TE and NT
* RX TimerEvent and IsdnMsg only for NT in order to enter user space NT stack
*/
void IsdnDeviceMgr::mgrThreadMainLoop()
{
	cpLog( LOG_DEBUG, "IsdnMgr waiting event....." );
	Sptr <PbxEvent> event =  getQueue()->getNext();

	/* timer has high priority */
	Sptr <PbxTimerEvent> ntTimerEvent = NULL;
	ntTimerEvent.dynamicCast(event);
	if( ntTimerEvent != 0)
	{
		cpLog(LOG_INFO,"NT Timer Event");
		processTimerEvent(ntTimerEvent);
		return ;
	}

	Sptr <PbxDeviceEvent> devEvent = NULL;
	devEvent.dynamicCast(event);
	if(devEvent!= 0 )
	{
		cpLog(LOG_INFO, "Msg Event ...");
		processMsgEvent( devEvent);
		return;
	}

	cpLog(LOG_ERR, "Invalidate Event found in ISDN-MGR");
}

int IsdnDeviceMgr::processMsgEvent(Sptr <PbxDeviceEvent> msgEvent)
{
	iframe_t *frm;
	int active = 0;
	int i;
	assert(msgEvent != 0);

	msg_t *msg = (msg_t *)msgEvent->getPrivate();
	assert(msg != 0);
	
	Sptr <EndPoint> ep = msgEvent->getEndPoint();

	if( ep!=0 )
	{/* L3 msg(CC_XXX) send by Gateway thread */
		Sptr <IsdnNtDevice> ntCtrl = NULL;
		ntCtrl.dynamicCast( ep->getCtrlDevice() );
		assert(ntCtrl != 0);

		return ntCtrl->enterStatckfromUpper( msg);
	}

	/* msg send by ISDN Device monitor thread */
	frm = (iframe_t *)msg->data;

	/* global prim */
	switch(frm->prim)
	{
		case MGR_INITTIMER | CONFIRM:
		case MGR_ADDTIMER | CONFIRM:
		case MGR_DELTIMER | CONFIRM:
		case MGR_REMOVETIMER | CONFIRM:
			cpLog(LOG_DEBUG_STACK,  "timer-confirm\n");
			as_msg_free(msg);
			return 0;
	}

	switch(frm->prim)
	{
		case  (MGR_TIMER | INDICATION):
		{
			cpLog(LOG_DEBUG_STACK,  "timer-indication\n");
#if 0			
			Sptr <CtrlDevice> ctrl = NULL;
			
			while( (ctrl=getNextDevice(ctrl) ) != 0)
			{
				Sptr<IsdnCtrlDevice> isdnCtrl = NULL;
				isdnCtrl.dynamicCast(ctrl);
				assert(isdnCtrl!= 0);
				ret = isdnCtrl->kernelTimerMsg(msg);
				if(ret != AS_MSG_CONTINUE)
					break;
			}
#endif			
			as_msg_free(msg);
			return 0;
		}	
	}
	
//	cpLog(LOG_DEBUG_STACK,  " RX!!\tprim : %x \taddress : %x \tdinfo : %x \tlength : %d\n",  frm->prim, frm->addr&IF_ADDRMASK, frm->dinfo, frm->len);

	/* find the port */
	for(i=0;i < getDeviceCount()/*ISDN_PORT_COUNT*/; i++)
	{
		Sptr <CtrlDevice> dev = getDeviceByAddress(frm->addr );
//		if (  (frm->addr&IF_ADDRMASK) == dev->getAddress()   )
		if ( dev !=0 )
		{
			Sptr <IsdnCtrlDevice> isdnDev ;
			isdnDev.dynamicCast(dev);
			if ( isdnDev == 0 )
			{
				assert(0);
			}

			/* L2 and other message which not associated call */
			active = isdnDev->hardwareMain( msg) ;
//			if(active==AS_MSG_HANDLED ||active == AS_MSG_NEED_FREE )
			if(active== AS_MSG_NEED_FREE )
			{
				as_msg_free(msg);
			}
			else if(active == AS_MSG_HANDLED )
			{
				cpLog(LOG_DEBUG, "MSG handled, not free is need");
			}
			else if(active== AS_MSG_CONTINUE)
			{/* this msg is not handled by D and B Channel, some error happened
			* Code should be enhanced in future, here
			*/
				cpLog(LOG_ERR, "Unknown msg, need to check code carefully");
				as_msg_free(msg);
	//			assert(0);
			}
			else
			{
				cpLog(LOG_ERR, "msg handler return error");
			}
			return 0;
//			return active ;
		}
		else
		{
			cpLog(LOG_ERR, "No IsdnCtrlDevice for frame with address 0x%x", frm->addr);
			assert(0);
		}
	} 
	
	cpLog(LOG_ERR, "unhandled message: prim(0x%x) addr(0x%x) msg->len(%d)\n", frm->prim, frm->addr, msg->len);

	as_msg_free(msg);
	return 0;
}

/* TimerEvent are only used in NT device */
int IsdnDeviceMgr::processTimerEvent(Sptr <PbxTimerEvent>  ntTimerEvent)
{
	assert(ntTimerEvent!=0);
	Sptr <EndPoint> ep = ntTimerEvent->getEndPoint();
	assert(ep!=0);

	Sptr <IsdnCtrlDevice> isdnCtrl = NULL;
	isdnCtrl.dynamicCast(ep->getCtrlDevice() );
	assert(isdnCtrl != 0);
	isdnCtrl->timerEventExpired(ntTimerEvent );
	return 0;
}

int IsdnDeviceMgr::reportMsgEvent(msg_t *msg, Sptr <EndPoint> ep)
{
	Sptr <PbxDeviceEvent> devEvent = new PbxDeviceEvent();
	if(ep != 0)
		devEvent->setEndPoint( ep );
	devEvent->setPrivate((void *)msg);

	PbxManager::reportEvent(devEvent);
	return 0;
}

