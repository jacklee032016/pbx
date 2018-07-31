/*
* $Id: PstnDeviceMgr.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "PstnDeviceMgr.hxx"
#include "PbxAgent.hxx"
#include "PbxConfiguration.hxx"
#include "DialingValidator.hxx"

#include "CallTimerEvent.hxx"

#include "PstnCtrlDevice.hxx"

#include "PstnStateBuilder.hxx"
#include "EpStateMachine.hxx"

using namespace Assist;

as_span_t  *as_check_hardware(const char *name, assist_app_type appType );

PstnDeviceMgr::PstnDeviceMgr(PbxAgent* agent )
	:DeviceMgr(agent)
{
	mySupportSecondDial = true;
	myDialMethodType = PSTN_DIAL;
	
	EpStateMachine *stateMachine = new EpStateMachine(this);

	stateMachine->addState( new PstnStateIdle );
	stateMachine->addState( new PstnStateDialing);
	stateMachine->addState( new PstnStateRinging);
	stateMachine->addState( new PstnStateTrying);
	stateMachine->addState( new PstnStateInCall);
	stateMachine->addState( new PstnStateError);
	stateMachine->addState( new PstnStateCar);
	stateMachine->addState( new PstnStateCallId);
	stateMachine->addState( new PstnStateIncoming);
	
	setPbxStateMachine( stateMachine);

	if ( init() )
		assert(0);
}

PstnDeviceMgr::~PstnDeviceMgr()
{
	destory();
}

void  PstnDeviceMgr::mainLoop (void* params)
{
	myMainLoop(params); 
}

void PstnDeviceMgr::myMainLoop(void *params)
{
	fd_set readfds;

	int i;
	as_device_t *dev;
	
	struct timeval tv;
	int retval;
	int maxFd = 128;  
	
	// reset file descriptor
	FD_ZERO(&readfds);
	addToFdSet(&readfds);

	tv.tv_sec = 0;
	tv.tv_usec = 500;

	/* monitor the exception fdset with only assist device when unlocked */
	if((retval = select(maxFd, 0, 0, &readfds, &tv)) < 0)
	{
		cpLog( LOG_ERR, "select() returned error %d", retval );
	}
	else
	{
		for(i=0; i< span->count; i++)
		{
			dev = span->devs[i];
			dev->ops->check_status( dev );
		}

	}

	/* only when EP is STATE_RING, following event may be report to StateMachine */
	for(i=0; i< span->count; i++)
	{
		dev = span->devs[i];
		dev->ops->get_status( dev);
#if 0		
		state = dev->ops->get_status( dev );
		
		if(state == AS_DEVICE_STATE_IDLE)
		{
			reportPEvent( DeviceEventIdle, dev->id);
			cpLog(LOG_DEBUG, "device '%s' idle", dev->name);
		}		
		else if(state == AS_DEVICE_STATE_BUSY)
		{
			reportPEvent( DeviceEventBusy, dev->id);
			cpLog(LOG_DEBUG, "device '%s' busy", dev->name);			
		}
#endif		
	}	
	
}

void  PstnDeviceMgr::downProcessMain (void* parms)
{
	
	cpLog(LOG_DEBUG, "1");
	Sptr <PbxDeviceEvent> event;

	Sptr <CtrlDevice> dev = NULL;
	while((dev = getNextDevice( dev) ) != 0 )
	{
#if 0	
		Sptr <IsdnCtrlDevice> isdnCtrl;
		isdnCtrl.dynamicCast(dev);
		assert(isdnCtrl != 0);
#endif		
		//isdnCtrl->downProcessMain( event);
	}
}

int PstnDeviceMgr::init()
{
	as_device_t *dev;

	//cout << endl;
	span = as_check_hardware("PSTN", assist_app_slic );
	assert(span!=0);
	if (span== 0)
	{
		cpLog(LOG_EMERG, "Can't Initialization PSTN devices");
		return -1;
	}

	cpLog(LOG_DEBUG, "PSTN Device : %d", span->count);

	for(int i=0; i<span->count; i++)
	{
		dev = span->devs[i];		
		CtrlDevice *pdev = new PstnCtrlDevice(this, i, dev, "PSTN");
		addDevice(pdev);
		Sptr <EndPoint> ep = pdev->getNextEP(0);
		assert(ep!= 0);
		dev->private_data = (void *)ep.getPtr();
	}

	return 0;
}

void PstnDeviceMgr::destory(void)
{
	cpLog(LOG_DEBUG, "Close SLIC %d port devices......\n", getDeviceCount() );
}


/// Process a msg received from an session
void PstnDeviceMgr::myProcessHwEvent( Sptr <PbxHwEvent> event )
{
	cpLog(LOG_ERR, "PbxHwEvent arrived");
}

int PstnDeviceMgr::addToFdSet (fd_set* fd)
{
	int i;
	for(i=0; i< span->count; i++)
	{
		FD_SET((span->devs[i])->fd,  fd);
	}
	return 0;
}

void PstnDeviceMgr::reportHardwareDtmfEvent(as_device_t *dev)
{
	CtrlDevice* ctrldev = static_cast<CtrlDevice*>(dev->private_data);
	Sptr <PstnEndPoint> pstnep;
	pstnep.dynamicCast(ctrldev->getNextEP(0));
	assert(pstnep != 0);//
	unsigned char dtmf = pstnep->reportDtmfDigit();

	if(dtmf != 'X' && dtmf!=0xff )
	{
		cpDebug(LOG_DEBUG, "DTMF '%c'(0x%x)", dtmf, dtmf);
		pstnep->genDtmfEvent( dtmf);
	}	
}


Sptr <EndPoint>  PstnDeviceMgr::lookupEndPointByNumber(const char *extPhoneNumber)
{
	int index = 0;
	Sptr <CtrlDevice> ctrl;
#if 9	
	string ext = extPhoneNumber;
#endif

#if 0	
	while ((ctrl = getNextDevice(index)) != NULL)
	{
		//if (strcmp(ctrl->getNextEP(0)->getPhoneNumber(), extPhoneNumber) == 0)
		//	return ctrl->getNextEP(0);

		Sptr <EndPoint> ep = ctrl->lookupEPByNumber(extPhoneNumber);
		if (ep != 0)
		{
			return ep;
		}		
		++index;
	}
#endif

	index = 0;
	while ((ctrl = getNextDevice(index)) != NULL)
	{
		Sptr <EndPoint> pep =	ctrl->getNextEP(0);
		assert(pep != 0);
		DialMethodsType method = PbxAgent::instance()->getGateway()->getDialingValidator()->checkDialMethod(extPhoneNumber);
		if (!index && ( method!= getDialMethodType()) ) 
		{
			cpDebug(LOG_DEBUG, "'%s' is not a PSTN DIAL(Method is %d)", extPhoneNumber, method);
			break;
		}	
		if ( !pep->checkExt() )
		{/* FXO device */
			if (pep->getState()->getStateType() == PSTN_EP_STATE_IDLE )
			{
				return pep;
			}
		}
		++index;
	}
	return NULL;
}

