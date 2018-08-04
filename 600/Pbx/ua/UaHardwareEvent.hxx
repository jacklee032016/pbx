#ifndef UaHardwareEvent_H
#define UaHardwareEvent_H
/*
 * $Id: UaHardwareEvent.hxx,v 1.2 2007/03/02 04:19:29 lizhijie Exp $
 */

#include <vector>

//#include <Sptr.hxx>
//#include <Fifo.h>

#include "UaDeviceEvent.hxx"
#include "SipProxyEvent.hxx"


namespace Vocal
{

///
enum HardwareMsgType
{
    HardwareSignalType,
    HardwareAudioType,
    	HardwareTimeoutType  /* for PBX virtual off-hook */
};

///
enum HardwareAudioRequestType
{
    AudioStart,
    AudioStop,
    AudioSuspend,
    AudioResume
};

//added by lijie 2005-06-03
enum AudioActionType
{
	StartGreeting,
	StopGreeting,
	StartRingBack,
	StopRingBack,
	StartBusyTone,
	StopBusyTone,
	StartCallWaiting,
	StopCallWaiting,
	StartDialerrTone = 100,
	StartConnerrTone,
      StartLinedropTone,
	ActionNull
};
//added ends here

#define RGW_HOST_STRING_LEN (128)
#define RGW_MODE_LEN (128)

///
struct HardwareAudioRequest
{
    HardwareAudioRequestType type;
    int localPort;
    int remotePort;
    char localHost[RGW_HOST_STRING_LEN];
    char remoteHost[RGW_HOST_STRING_LEN];
    char mode[RGW_MODE_LEN];  // should this be an enum ???
    bool echoCancellation;
    int rtpPacketSize;
    //modified by lijie 	
    //bool sendRingback;	
    AudioActionType actionType;
};


///
typedef union
{
    DeviceSignalType signal;
    struct HardwareAudioRequest request;
} HardwareMsgUnion;


/// Message for hardware
class UaHardwareEvent : public SipProxyEvent
{
    public:
        ///
        UaHardwareEvent( const Sptr < Fifo < Sptr < SipProxyEvent > > > outputFifo );
        ///
        virtual ~UaHardwareEvent();
        ///
        virtual const char* const name() const;

	void startTimer(Sptr < SipProxyEvent > newEvent, const int timerMs);
        //TODO move public data to protected area
        ///
        HardwareMsgType type;
        ///
        HardwareMsgUnion signalOrRequest;
        ///
        vector < string > dataList;
};
 
}

#endif
