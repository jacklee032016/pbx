/*
* $Id: CtrlDevice.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "Sptr.hxx"
#include "cpLog.h"
#include <unistd.h>
#include <assert.h>

#include "EndPoint.hxx"
#include "DeviceMgr.hxx"
#include "CtrlDevice.hxx"

using namespace Assist;

CtrlDevice::CtrlDevice(DeviceMgr *_parent , int _index, char *_name)
	:myParent(_parent),
	myIndex(_index)
{
	assert(myParent != 0);
	sprintf(myName, "%s-%d", _name, _index);
	cpLog(LOG_DEBUG, "create '%s' Device\n", myName );
	
}

CtrlDevice::~CtrlDevice()
{
	cpLog(LOG_DEBUG, "Destorying %s.....", getName() );
}



#if WITH_DEBUG
const char *CtrlDevice::debugInfo()
{
	char *buf, *tmp;
	int length = 0;
	int i = 0;

	buf = ( char *)malloc(4096);
	assert(buf != 0);
	memset(buf, 0 ,sizeof(buf) );
	
	length += sprintf(buf+length, "\r\n\tCtrlDevice %s(index : %d) Info :\n", getName(), getIndex() );

	Sptr <EndPoint> ep = 0;
	i = 0;
	while( (ep = getNextEP( ep) ) != 0)
	{
		i++;
//		length += sprintf(buf+length, "\tEndPoint %d Info :\n" , i) ;
		tmp = (char *)ep->debugInfo();
		length += sprintf(buf+length, "%s",  tmp);
		free(tmp);
		tmp = NULL;
	}

	Sptr <AudioChannel> audio = 0;
	while( (audio = getNextAudio(audio) ) != 0)
	{
		i++;
//		length += sprintf(buf+length, "\tAudio Channel %d Info :\n" , i) ;
		tmp = (char *)audio->debugInfo();
		length += sprintf(buf+length, "%s", tmp);
		tmp = NULL;
		free(tmp);
	}
	length += sprintf(buf+length, "\r\n\r\n" );

	return buf;
}
#endif

void CtrlDevice::processAudio()
{
	Sptr <AudioChannel> audio = 0;
	
//	myAudioMutex.lock();
	
	while( (audio=getNextAudio(audio)) != 0)
	{
//		cpLog(LOG_DEBUG, "Audio Channel[%d] in device %s is used", i, getName() );
		if( audio->canProcess() == true)
		{
			audio->processAudio();
		}
//		cpLog(LOG_DEBUG, "Audio Channel[%s] in device %s is used ,state is %d, peer state is %d", audio->getName(), getName(), audio->getState(), audio->getPeerState() );
	}
	
//	myAudioMutex.unlock();
}

int CtrlDevice::startup()
{
	return 0;
}

/* this is a default implement use for End device with phone number, such as FXS 
* for FXO, TE and NT, it must be overload 
*/
Sptr <EndPoint> CtrlDevice::lookupEPByNumber(const char *_number)
{
	int position = PbxConfiguration::instance()->getAccessLength();
	if(strncmp(myPhoneNumber, _number + position, strlen(myPhoneNumber) ) == 0)
	{
		return getNextEP( NULL);
	}

	return NULL;
}

void CtrlDevice::setExtPhoneNumber(const char *_extPhone)
{
	memset(myPhoneNumber, 0 ,VPBX_NAME_LENGTH);
	sprintf(myPhoneNumber, "%s", _extPhone);
}

Sptr <DeviceMgr> 	CtrlDevice::getParent()
{
	return myParent;
}

Sptr <EndPoint> CtrlDevice::getNextEP( Sptr <EndPoint> _ep)
{
	if(_ep == 0)
	{	
		return myEndPoint;
	}
	else
	{	
		return 0;
	}
}

Sptr <AudioChannel> CtrlDevice::getNextAudio( Sptr <AudioChannel> _audio)
{
	if(_audio == 0)
		return myAudioChan;
	else
		return 0;
}


