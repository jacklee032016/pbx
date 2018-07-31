/*
* $Id: VoipCtrlDevice.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "VoipCtrlDevice.hxx"
#include "VoipDeviceMgr.hxx"
#include "VoipAudioChannel.hxx"

using namespace Assist;

VoipCtrlDevice::VoipCtrlDevice(DeviceMgr  *_parent , Sptr <SipTransceiver> sipStack, int _myIndex,  char *_name, bool callLegHistory )
	:CtrlDevice(_parent, _myIndex, _name)
{
	myEndPoint = new VoipEndPoint(this,sipStack, callLegHistory );
	myAudioChan = new VoipAudioChannel(this, 0);
	myEndPoint->setAudio(myAudioChan);
	myAudioChan->setEndPoint(myEndPoint);
	assert(myEndPoint != 0);
	assert(myAudioChan != 0);
	
}

VoipCtrlDevice::~VoipCtrlDevice()
{
}

int VoipCtrlDevice::init( )
{
	return 0;
}

/* send HW command to device, called by DeviceCommandThread */
void VoipCtrlDevice::sendHwCommand(Sptr <PbxEvent> hwCommand)
{
}

Sptr <EndPoint> VoipCtrlDevice::lookupFreeEP()
{
	return myEndPoint;
}

Sptr <EndPoint> VoipCtrlDevice::lookupEPByPhoneNumber(const char *_number)
{
	int position = PbxConfiguration::instance()->getAccessLength();
	if(strncmp(myPhoneNumber, _number + position, strlen(myPhoneNumber) ) == 0)
	{
		if(myEndPoint->getState()->getStateType() == 0)
		return myEndPoint;
	}

	return NULL;
}

