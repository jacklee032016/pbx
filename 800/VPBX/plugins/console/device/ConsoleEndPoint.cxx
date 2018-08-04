/*
* $Id: ConsoleEndPoint.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "DigitEvent.hxx"
#include "PbxDeviceEvent.hxx"

#include "PbxAgent.hxx"

#include "assist_lib.h"

#include "ConsoleCtrlDevice.hxx"
#include "ConsoleEndPoint.hxx"

using namespace Assist;

ConsoleEndPoint::ConsoleEndPoint(  CtrlDevice *_ctrlDev, console_dev_t  isMaster )
	:EndPoint( _ctrlDev )
{
	myDevType = isMaster;
	setState( findState( CONSOLE_EP_STATE_IDLE) );
	sprintf(myName, "CONSOLE-EP(%s)" ,isMaster==CONSOLE_MASTER?"Master" :"Slave");

#if 0
	if(isMaster == CONSOLE_MASTER)
	{	
	cpLog(LOG_DEBUG, "ConsoleEndPoint");
		mySlave = new ConsoleEndPoint( _ctrlDev, CONSOLE_SLAVE);
	cpLog(LOG_DEBUG, "ConsoleEndPoint");
	}
#endif
	cpLog(LOG_DEBUG, "ConsoleEndPoint");
}

ConsoleEndPoint::~ConsoleEndPoint(void)
{
	cpLog(LOG_DEBUG, "Destorying ConsoleEndPoint %s.....", getName() );
}

#if WITH_DEBUG
const char *ConsoleEndPoint::debugInfo()
{
	char *buf, *tmp;
	int length = 0;
	
	buf = (char *)malloc( 4096 );
	assert( buf != 0);
	memset(buf, 0 , sizeof(buf) );
	
	tmp = (char *)EndPoint::debugInfo();
	length += sprintf(buf+length, "%s", tmp );
	free(tmp);
	tmp = NULL;
	return buf;
}
#endif

Sptr <ConsoleEndPoint> ConsoleEndPoint::getSlave()
{
	return mySlave;
}

void ConsoleEndPoint::setSlave(Sptr <ConsoleEndPoint> slave)
{
	mySlave = slave;
}

void ConsoleEndPoint::reset()
{
	EndPoint::reset();
	myAudio->deactivate();
}

void ConsoleEndPoint::stopDialTone()
{
	myAudio->stopDialTone();
}

