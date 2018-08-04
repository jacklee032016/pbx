/*
 * $Id: AssistInvalidDevice.cxx,v 1.1.1.1 2006/11/29 09:50:50 lizhijie Exp $
*/

#include "UaConfiguration.hxx"
#include "AssistInvalidDevice.hxx"
#include "AssistNullDevice.hxx"
#include <sys/time.h>
#include "g711.h"
#include "cpLog.h"

using namespace Vocal;

AssistInvalidDevice::AssistInvalidDevice( AssistNullDevice *agent, char *name, device_type_t type,  int deviceId, Sptr < Fifo < Sptr < SipProxyEvent > > > sessionQ)
	:AssistDevice( agent,  name, type, deviceId, sessionQ )
{

}


AssistInvalidDevice::~AssistInvalidDevice()
{
}

void AssistInvalidDevice::provideDialToneStart()
{
	return;
}

void AssistInvalidDevice::provideQueryStatus()
{
	return;
}
void AssistInvalidDevice::provideDialToneStop()
{
	return;
}

void AssistInvalidDevice::provideRingStart()
{
	return;	
}

void AssistInvalidDevice::provideRingStop( )
{
	return;
}

void AssistInvalidDevice::provideLocalRingbackStart()
{
	return;
}

void AssistInvalidDevice::provideLocalRingbackStop( )
{
	return;
}

void AssistInvalidDevice::provideRemoteRingbackStart()
{
	return;
}

void AssistInvalidDevice::provideRemoteRingbackStop( )
{
	return;
}
void AssistInvalidDevice::provideBusyToneStart( )
{
	return;
}

void AssistInvalidDevice::provideBusyToneStop()
{
	return;
}

void AssistInvalidDevice::provideFastBusyToneStart( )
{
	return;
}

void AssistInvalidDevice::provideFastBusyToneStop( )
{
	return;
}

void AssistInvalidDevice::provideCallWaitingBeepStart()
{
	return;
}

void AssistInvalidDevice::provideCallWaitingBeepStop()
{
	return;
}

void AssistInvalidDevice::ProcessRTP()
{
	return;
}

unsigned char  AssistInvalidDevice::reportDtmfDigit( )
{
	return 'X';
}

bool AssistInvalidDevice::emitDtmfSignal(char signal)
{
	return false;
}

int AssistInvalidDevice::openDeviceFile()
{
	return 0;
}
		
void AssistInvalidDevice::closeDeviceFile()
{
 	return;
}
 

