/*
* $Id: ConsoleDeviceMgr.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "PbxAgent.hxx"

#include "EpStateMachine.hxx"
#include "ConsoleStateBuilder.hxx"
#include "ConsoleCtrlDevice.hxx"
#include "ConsoleDeviceMgr.hxx"

using namespace Assist;

ConsoleDeviceMgr::ConsoleDeviceMgr(PbxAgent *agent )
	:DeviceMgr(agent)
{
	mySupportSecondDial = true;
	myDialMethodType = CONSOLE_DIAL;

	EpStateMachine *stateMachine = new EpStateMachine(this);

	stateMachine->addState( new ConsoleStateIdle );
	stateMachine->addState( new ConsoleStateInAlerting);
	stateMachine->addState( new ConsoleStateInGreeting);
	stateMachine->addState( new ConsoleStateOutCalling);
	stateMachine->addState( new ConsoleStateOutAlerting);
	stateMachine->addState( new ConsoleStateConnect);

	setPbxStateMachine( stateMachine);
	
	if ( init() )
		assert(0);

}

int ConsoleDeviceMgr::init()
{
	int i;
	CtrlDevice *ctrl;
	for(i=0;i < PbxConfiguration::instance()->getPbxCount(); i++)
	{
		 ctrl =  new ConsoleCtrlDevice(this, i);
		 assert( ctrl != 0 );
		 addDevice( ctrl);
	}

	setDeviceCount(i );

	return 0;
}

ConsoleDeviceMgr::~ConsoleDeviceMgr()
{
	cpLog(LOG_DEBUG, "Destorying.....");
	destory();
}

void ConsoleDeviceMgr::destory(void)
{
	cpLog(LOG_DEBUG, "Close Console devices......" );
}

void ConsoleDeviceMgr::myMainLoop(void *params)
{
	return;	
}

void ConsoleDeviceMgr::myProcessEvent( Sptr<PbxEvent> event )
{
	cpLog( LOG_DEBUG, "No event process is need in ConsoleDeviceMgr");
}

Sptr <CtrlDevice> ConsoleDeviceMgr::getDeviceByAddress(unsigned int address )
{
	Sptr <CtrlDevice> dev = NULL;

	while ( (dev = getNextDevice(dev)) != 0  )
	{
			return dev;
	}

	return NULL;	
}

/// Process a msg received from an session
void ConsoleDeviceMgr::myProcessHwEvent( Sptr <PbxHwEvent > event )
{
	cpLog(LOG_DEBUG, "No command send to hardware in ConsoleDeviceMgr");
	return;
}

/* called by callScheduler to routing a call */		
Sptr <EndPoint> ConsoleDeviceMgr::lookupEndPointByNumber(const char *extPhoneNumber)
{
	Sptr <CtrlDevice> ctrl = NULL;

	while( (ctrl=getNextDevice( ctrl)) != 0)
	{
		Sptr <EndPoint> ep = ctrl->lookupFreeEP();
		if( ep != 0)
			return ep;
	}

	return 0;
}

Sptr <EndPoint> ConsoleDeviceMgr::findFreeEP()
{
	Sptr <CtrlDevice> ctrl = NULL;

	cpLog(LOG_DEBUG, "find a EP in %s", getName() );
	while( (ctrl=getNextDevice( ctrl)) != 0)
	{
		Sptr <EndPoint> ep = ctrl->lookupFreeEP();
		if( ep != 0)
			return ep;
	}

	return 0;
}

