/*
* $Id: PstnCtrlDevice.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "PstnEndPoint.hxx"
#include "PstnDeviceMgr.hxx"
#include "PstnAudioChannel.hxx"
#include "PbxConfiguration.hxx"

#include "DigitEvent.hxx"
#include "PstnCtrlDevice.hxx"

using namespace Assist;


PstnCtrlDevice::PstnCtrlDevice(DeviceMgr  *_parent ,  int _portnum, as_device_t *asdev, char *_name )
	:CtrlDevice(_parent, _portnum, asdev->name)
{
	Sptr <PstnEndPoint> pep = new PstnEndPoint(this, asdev);
	myEndPoint = pep;
	myAudioChan = new PstnAudioChannel(this, pep->getMyDev()->fd );
	myEndPoint->setAudio(myAudioChan);
	myAudioChan->setEndPoint(pep);
	assert(myEndPoint != 0);
	
}

PstnCtrlDevice::~PstnCtrlDevice()
{
}

int PstnCtrlDevice::init( )
{
	return 0;
}

/* called by processSessionMsg */
int PstnCtrlDevice::provideSignal( DeviceSignalType signal )
{
	as_device_t 			*dev;
	Sptr <PstnEndPoint> pep;
	pep.dynamicCast(myEndPoint);
	assert(pep!= 0);

	dev = pep->getMyDev();
	switch( signal )
	{
		case DeviceSignalQueryStatus:
			cpLog(LOG_DEBUG, "query status on device %s", dev->name);
			dev->ops->query_status(dev);
			break;		
		case DeviceSignalDialToneStart:
			cpLog(LOG_DEBUG, "dial start on device %s", dev->name );
			dev->ops->tone_dial_start( dev);
			break;
		case DeviceSignalDialToneStop:
			cpLog(LOG_DEBUG, "***  Stop dial tone on %s***" ,  dev->name);
			dev->ops->tone_dial_stop( dev);
			break;
		case DeviceSignalRingStart:
 			cpLog(LOG_DEBUG, "***  ------Start Ringing on %s ***" ,  dev->name );
			dev->ops->ring_start( dev, myEndPoint->getPeerNumber().c_str());
			break;
		case DeviceSignalRingStop:
			cpLog(LOG_DEBUG, "***  Stop Ringing on %s ***",  dev->name);
			dev->ops->ring_stop(dev);
			break;
		case DeviceSignalLocalRingbackStart:
			cpLog(LOG_DEBUG, "***  Start local ringback on %s ***",  dev->name);
			dev->ops->tone_ringback_start(dev);
			break;
		case DeviceSignalLocalRingbackStop:
			cpLog(LOG_DEBUG, "***  Stop local ringback on %s ***" , dev->name);
			dev->ops->tone_ringback_stop(dev);
			break;
		case DeviceSignalBusyStart:
			cpLog(LOG_DEBUG, "***  Start busy tone on %s ***" ,  dev->name);
			dev->ops->tone_busy_start(dev );
			break;
		case DeviceSignalBusyStop:
			cpLog(LOG_DEBUG, "***  Stop busy tone on %s ***" ,  dev->name);
			dev->ops->tone_busy_stop(dev );
			break;
		case DeviceSignalFastBusyStart:
			cpLog(LOG_DEBUG, "***  Start fast busy tone on %s ***" , dev->name);
			dev->ops->tone_fastbusy_start(dev );
			break;
		case DeviceSignalFastBusyStop:
			cpLog(LOG_DEBUG, "***  Stop fast busy tone on %s ***" ,  dev->name);
			dev->ops->tone_fastbusy_stop(dev);
			break;
		case DeviceSignalStartCallWaitingBeep:
			cpLog(LOG_DEBUG, "***  Start call waiting beep on %s***",  dev->name);
			dev->ops->tone_callwait_start(dev);
			break;
		case DeviceSignalStopCallWaitingBeep:
			cpLog(LOG_DEBUG, "***  Stop call waiting beep on %s***" ,  dev->name);
			dev->ops->tone_callwait_stop(dev);
			break;

		case DeviceSignalCar:
			cpLog(LOG_DEBUG, "***  FXS send CAR on %s***" ,  dev->name);
			dev->ops->query_channel(dev);
			break;
		case DeviceSignalCarCallerID:
			cpLog(LOG_DEBUG, "***  FXS send FSK CallerID on %s***" ,  dev->name);
			dev->ops->send_callid(dev, (char *)myEndPoint->getPeerNumber().c_str());
			break;
				
		case DeviceSignalFxoPrimAnswer:
			cpLog(LOG_DEBUG, "***  FXO send PrimaryAnswer on %s***" ,  dev->name);
			dev->ops->send_primary_answer(dev);
			break;
		case DeviceSignalFxoIncomingSucess:
			cpLog(LOG_DEBUG, "***  send FXO IncomingSuccess on %s***" ,  dev->name);
			dev->ops->send_incoming_success(dev );
			break;
				
		default:
			cpLog(LOG_ERR, "Requested signal %d not supported by hardware", signal);
			return -1;
	}

	return 0;
}

/* send HW command to device, called by DeviceCommandThread */
void PstnCtrlDevice::sendHwCommand(Sptr <PbxEvent> hwCommand)
{
	if (hwCommand == 0) 
		return;

	Sptr <DigitEvent> digit;
	digit.dynamicCast(hwCommand);
	if(digit != 0)
	{
		as_device_t 			*dev;
		Sptr <PstnEndPoint> pep;
		pep.dynamicCast(myEndPoint);
		assert(pep!= 0);
		dev = pep->getMyDev();
		char num[10];
		num[0] = digit->myDigit;
		num[1] = 0;
		as_dsp_play_dtmf(dev->dsp, dev->fd, num);
		
		return;
	}
	
	Sptr <PbxHwEvent> event = 0;
	event.dynamicCast(hwCommand);
	if(event == 0)
	{
		cpLog(LOG_WARNING, " not a hwEvent");
		return;
	}
	
	cpLog(LOG_DEBUG, "Got message type: '%s' from PstnEndPoint StateMachine", (event->type==HardwareSignalType)? "Signal":"Audio"  );
	switch( event->type )
	{
		case HardwareSignalType:
		{
			if( event->signalOrRequest.signal == DeviceSignalFwding )
			{
			}
			else
			{
				provideSignal((event->signalOrRequest).signal );
			}
			break;
		}	
		case HardwareTimeoutType:
		{
			//cpDebug(LOG_DEBUG, "Got TimerOut event with deviceId %d", event->getDeviceId() );
			//providePbxTimerEvent(msg);
			break;
		}	
		default:
			cpLog( LOG_ERR, "Unknown session message: %d", event->type );
			break;
	}
	
}

Sptr <EndPoint> PstnCtrlDevice::lookupFreeEP()
{
	return myEndPoint;
}

Sptr <EndPoint> PstnCtrlDevice::lookupEPByPhoneNumber(const char *_number)
{
	int position = PbxConfiguration::instance()->getAccessLength();
	if(strncmp(myPhoneNumber, _number + position, strlen(myPhoneNumber) ) == 0)
	{
		if(myEndPoint->getState()->getStateType() == 0)
		return myEndPoint;
	}

	return NULL;
}

