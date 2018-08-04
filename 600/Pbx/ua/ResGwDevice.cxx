/*
 * $Id: ResGwDevice.cxx,v 1.2 2007/03/02 04:19:29 lizhijie Exp $
 */

#include "global.h"
#include "ResGwDevice.hxx"
#include "UaConfiguration.hxx"

#include "UaTimerEvent.hxx"

#include "pbx_globals.h"
#include "AssistPbxDevice.hxx"

#ifndef NO_POLICY
#include "PepAgent.hxx"
#endif

using namespace Vocal;

ResGwDevice::ResGwDevice( const char* deviceName,
                          Sptr < Fifo < Sptr < SipProxyEvent > > > inputQ,
                          Sptr < Fifo < Sptr < SipProxyEvent > > > outputQ )
        : sessionQ(outputQ)
//        myEntryState( EntryStateTelephoneUI )
{

//	digitCollector = new DigitCollector(AS_DEVICE_NOT_DEFINED, outputQ );
#if 0
	callWaitingIdList = new CallWaitingIdList;

	callIdList = new CallIdList;
#endif

	
#ifndef NO_POLICY
    if ( UaConfiguration::instance()->getUsePolicyServer() )
    {
        try
        {
            string psHost = UaConfiguration::instance()->getProvisioningHost();
            int psPort = UaConfiguration::instance()->getProvisioningPort();

            PepAgent::instance( psHost.c_str(), psPort, psHost.c_str(), psPort);
        }
        catch (VNetworkException & e)
        {
            cpDebug(LOG_DEBUG, "SUA PEPAgent error");
        }
    }
#endif
}


ResGwDevice::~ResGwDevice()
{
}

//removed by liijie
#if 0
void ResGwDevice::addCallWaitingId( const Sptr<SipCallId> newCallId )
{
    if( newCallId == 0 )
    {
        cpDebug(LOG_DEBUG, "No null id is added" );
        return ;
    }

    callWaitingIdList->push_back( newCallId );
    cpDebug(LOG_DEBUG, "added callId %s", newCallId->encode().logData());
}


Sptr<SipCallId> ResGwDevice::getCallWaitingId() const
{
    if ( callWaitingIdList->empty() )
    {
        cpDebug(LOG_DEBUG, "call waiting list empty" );
        return 0;
    }

    return callWaitingIdList->front();
}


void ResGwDevice::removeCallWaitingId( const SipCallId& callId )
{
    cpDebug(LOG_DEBUG, "try to remove callId %s", callId.encode().logData() );

    CallWaitingIdList::iterator iter = callWaitingIdList->begin();

    while ( iter != callWaitingIdList->end() )
    {
        if ( *(*iter) == callId )
        {
            callWaitingIdList->erase(iter);
            cpDebug(LOG_DEBUG, "callId %s removed", callId.encode().logData() );
            break;
        }
        iter++;
    }

    return ;
}

bool ResGwDevice::isOnCallWaiting(const SipCallId &callid)
{
    bool ret = false;

    CallWaitingIdList::iterator iter = callWaitingIdList->begin();

    while ( iter != callWaitingIdList->end() )
    {
        if ( *(*iter) == callid )
        {
            ret = true;
            break;
        }
        iter++;
    }
    return ret;
}
#endif

/* only called by DeviceThread */
void ResGwDevice::processSessionMsg( Sptr<SipProxyEvent> event )
{
	cpDebug(LOG_DEBUG, "processSessionMsg %d", event->getDeviceId() );

	Sptr<UaHardwareEvent> msg;
	msg.dynamicCast( event );

	if( msg != 0 )
	{
		cpDebug(LOG_DEBUG, "Got message type: '%s' from WorkerThread", (msg->type==HardwareSignalType)? "Signal":"Audio"  );
		switch( msg->type )
		{
			case HardwareSignalType:
				if( msg->signalOrRequest.signal == DeviceSignalFwding )
				{
				#if 0
					if ( msg->dataList.size() != 3 )
					{
						cpLog( LOG_ERR, "message size is wrong: %d",  msg->dataList.size() );
					}
					else
					{   /** This was done for voicemail loadgen
				*  testing.  We can remove this in future releases.  (Just let it be for now.)   */
						string calleeNum = msg->dataList[2];
						string callerNum = msg->dataList[1];
						string reason = msg->dataList[0];
						provideCallInfo(callerNum, calleeNum, reason);
					}
				#endif 
				}
				else
				{
					provideSignal((msg->signalOrRequest).signal,event->getDeviceId());
				}
				break;
			case HardwareAudioType:
				switch ((msg->signalOrRequest).request.type)
				{
					case AudioStart:
						audioStart((msg->signalOrRequest).request,event->getDeviceId());
						break;
					case AudioStop:
						audioStop(event->getDeviceId());
						break;
					case AudioSuspend:
						audioSuspend(event->getDeviceId());
						break;
					case AudioResume:
						audioResume((msg->signalOrRequest).request,event->getDeviceId());
						break;
					default:
						cpLog( LOG_ERR, "Unknown audio request: %d", (msg->signalOrRequest).request.type );
				}
				break;
			case HardwareTimeoutType:
//				cpDebug(LOG_DEBUG, "Got TimerOut event with deviceId %d", event->getDeviceId() );
				providePbxTimerEvent(msg);
				break;
			default:
				cpLog( LOG_ERR, "Unknown session message: %d", msg->type );
				return;//break;
		}
	}

}



/* called by processSessionMsg */
int ResGwDevice::provideSignal( DeviceSignalType signal , int deviceId)
{
    cpDebug(LOG_DEBUG, "Checking signal %d from WorkerThread with deviceID %d", signal, deviceId);

    switch( signal )
    {
    //added by lijie to query device status
     case DeviceSignalQueryStatus:
		cpDebug(LOG_DEBUG, "query status on deviceID %d", deviceId);
        provideQueryStatus(deviceId);
        break;		
    case DeviceSignalDialToneStart:
		cpDebug(LOG_DEBUG, "dial start on deviceID %d", deviceId);
        provideDialToneStart(deviceId);
        break;
    case DeviceSignalDialToneStop:
        provideDialToneStop(deviceId);
        break;
    case DeviceSignalRingStart:
        provideRingStart(deviceId);
        break;
    case DeviceSignalRingStop:
        provideRingStop(deviceId);
        break;
    case DeviceSignalLocalRingbackStart:
        provideLocalRingbackStart(deviceId);
        break;
    case DeviceSignalLocalRingbackStop:
        provideLocalRingbackStop(deviceId);
        break;
    case DeviceSignalRemoteRingbackStart:
        provideRemoteRingbackStart(deviceId);
        break;
    case DeviceSignalRemoteRingbackStop:
        provideRemoteRingbackStop(deviceId);
        break;		
    case DeviceSignalBusyStart:
        provideBusyToneStart(deviceId);
        break;
    case DeviceSignalBusyStop:
        provideBusyToneStop(deviceId);
        break;
    case DeviceSignalFastBusyStart:
        provideFastBusyToneStart(deviceId);
        break;
    case DeviceSignalFastBusyStop:
        provideFastBusyToneStop(deviceId);
        break;
    case DeviceSignalStartCallWaitingBeep:
        provideCallWaitingBeepStart(deviceId);
        break;
    case DeviceSignalStopCallWaitingBeep:
        provideCallWaitingBeepStop(deviceId);
        break;
    case DeviceSignalNetDropTone:
    case DeviceSignalDialErrorTone:
    case DeviceSignalConnErrorTone:
	  provideLocalErrorTone(deviceId, signal);
	  break;
    case DeviceSignalDigit0:
    case DeviceSignalDigit1:
    case DeviceSignalDigit2:
    case DeviceSignalDigit3:
    case DeviceSignalDigit4:
    case DeviceSignalDigit5:
    case DeviceSignalDigit6:
    case DeviceSignalDigit7:
    case DeviceSignalDigit8:
    case DeviceSignalDigit9:
    case DeviceSignalDigitStar:
    case DeviceSignalDigitHash:
        provideDtmf( signal,deviceId );
        break;
    default:
        cpLog(LOG_ERR, "Requested signal %d not supported by hardware", signal);
        return -1;
    }
    return 0;
}

/* report hardware event to WorkerThread */
void ResGwDevice::reportEvent( DeviceEventType eventType , int deviceId )
{
	cpDebug(LOG_INFO, "Report device event of device %d to sessionQ: %d",deviceId, eventType );

	Sptr < UaDeviceEvent > event = new UaDeviceEvent( sessionQ );

	event->type = eventType;
#if 0	
	if( deviceId >= VIRTUAL_DEVICES_EPOCH && eventType >=DeviceEventDtmf0 )
	{
		event->callId = getPbxCallId(deviceId);
	}
	else
#endif		
#if 0 //removed by lijie 2005-06-03
	{
		event->callId = getCallId(deviceId);
	}
#endif
	event->setDeviceId(deviceId);

	sessionQ->add( event );
}
