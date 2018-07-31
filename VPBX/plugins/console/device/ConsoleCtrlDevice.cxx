/*
* $Id: ConsoleCtrlDevice.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "DeviceMgr.hxx"

#include "ConsoleEndPoint.hxx"

#include "ConsoleCtrlDevice.hxx"


using namespace Assist;

ConsoleCtrlDevice::ConsoleCtrlDevice(DeviceMgr  *_parent ,  int _index)
	:CtrlDevice(_parent, _index, "CONSOLE-DEV")
{
	init();
}

ConsoleCtrlDevice::~ConsoleCtrlDevice()
{
	cpLog(LOG_DEBUG, "Destorying.....");
}

int ConsoleCtrlDevice::init( )
{
	Sptr <ConsoleEndPoint> myMasterEp  = new ConsoleEndPoint( this, CONSOLE_MASTER);
	Sptr <ConsoleAudioChannel> myMasterAudio = new ConsoleAudioChannel(this, CONSOLE_MASTER );
	Sptr <ConsoleEndPoint> mySlaveEp = new ConsoleEndPoint( this, CONSOLE_SLAVE);
	Sptr <ConsoleAudioChannel> mySlaveAudio = new ConsoleAudioChannel( this, CONSOLE_SLAVE);
		
	cpLog(LOG_DEBUG, "ConsoleCtrlDevice");
	
	myMasterEp->setSlave( mySlaveEp);
	myMasterAudio->setSlave( mySlaveAudio );

	myMasterEp->setAudio( myMasterAudio);
	myMasterAudio->setEndPoint(myMasterEp);

	mySlaveEp->setAudio(mySlaveAudio);
	mySlaveAudio->setEndPoint(mySlaveEp);

	myEndPoint = myMasterEp;
	myAudioChan = myMasterAudio;
	cpLog(LOG_DEBUG, "ConsoleCtrlDevice");

	return 0;
}

/* called by DeviceManager */
int ConsoleCtrlDevice::hardwareMain(void *params)
{
	cpLog(LOG_DEBUG, "Console hardwareMain");
	return AS_MSG_CONTINUE;
}

/* send HW command to device, called by DeviceCommandThread */
void ConsoleCtrlDevice::sendHwCommand( Sptr <PbxEvent> hwCommand)
{
//	myActionMutex.lock();
	cpLog(LOG_DEBUG, "%s", getName());

	if(hwCommand->getPrivate()==0)
	{
		cpLog(LOG_ERR, "No params in HW command for ISDN device");
		return ;
	}

//	myActionMutex.unlock();
	return;
}

Sptr <EndPoint> ConsoleCtrlDevice::lookupFreeEP()
{
	Sptr <EndPoint> ep = NULL;
	while( (ep=getNextEP(ep))!= 0)
	{
		if(ep->getState()->getStateType()==CONSOLE_EP_STATE_IDLE )
		{	
			return ep;
		}
	}
	
	return 0;
}


/* this is a default implement use for End device with phone number, such as FXS 
* for FXO, TE and NT, it must be overload, this function never be called 
*/
Sptr <EndPoint> ConsoleCtrlDevice::lookupEPByPhoneNumber(const char *_number)
{
#if 0
	int position = PbxConfiguration::instance()->getAccessLength();

	if(strncmp(myPhoneNumber, _number + position, strlen(myPhoneNumber) ) == 0)
	{
		if(myEndPoint->getState()->getStateType() != 0)/* busy */
			return myEndPoint;
		return NULL;
	}
#endif
	return NULL;
}

int ConsoleCtrlDevice::timerEventExpired(Sptr <PbxTimerEvent> timerEvent)
{
	return 0;
}

Sptr <ConsoleEndPoint> ConsoleCtrlDevice::getMasterEndPoint()
{
	Sptr <ConsoleEndPoint> cep;
	cep.dynamicCast(myEndPoint);
	assert(cep != 0);
	return cep;
}

Sptr <ConsoleAudioChannel> ConsoleCtrlDevice::getMasterAudioChannel()
{
	Sptr <ConsoleAudioChannel> cAudio;
	cAudio.dynamicCast(myAudioChan);
	assert(cAudio != 0);
	return cAudio;
}


Sptr <ConsoleEndPoint> ConsoleCtrlDevice::getSlaveEndPoint()
{
	Sptr <ConsoleEndPoint> cep = getMasterEndPoint();
	return cep->getSlave();
}

Sptr <ConsoleAudioChannel> ConsoleCtrlDevice::getSlaveAudioChannel()
{
	Sptr <ConsoleAudioChannel> cAudio = getMasterAudioChannel();
	return cAudio->getSlave();
}

