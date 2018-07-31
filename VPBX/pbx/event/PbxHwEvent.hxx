#ifndef __PBX_HW_EVENT_HXX__
#define __PBX_HW_EVENT_HXX__
/*
 * $Id: PbxHwEvent.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "PbxEvent.hxx"

namespace Assist
{

typedef enum
{
	DeviceSignalNULL,
	DeviceSignalDialToneStart,
	DeviceSignalDialToneStop,
	DeviceSignalRingStart,
	DeviceSignalRingStop,
	DeviceSignalRemoteRingbackStart,
	DeviceSignalRemoteRingbackStop,
	DeviceSignalLocalRingbackStart,		/* 7 */
	DeviceSignalLocalRingbackStop,
	DeviceSignalBusyStart,
	DeviceSignalBusyStop,
	DeviceSignalFastBusyStart,
	DeviceSignalFastBusyStop,
	DeviceSignalErrorToneStart,
	DeviceSignalErrorToneStop,

	DeviceSignalQueryStatus,

	DeviceSignalFwding,
	DeviceSignalStartCallWaitingBeep,
	DeviceSignalStopCallWaitingBeep,

	/* following is added for Japan CallerID */
	DeviceSignalCar,						/* send CAR command to DeviceCommandThread to put CAR on phone */
	DeviceSignalCarCallerID,				/* after primaryAnswer, send FSK modem data */
	
	DeviceSignalFxoPrimAnswer,			/* send out PrimaryAnswer command reponsing the rx CAR */
	DeviceSignalFxoIncomingSucess		/* send out IncomingSucess command response of FSK demodulation ended */
	
} DeviceSignalType;


//#define	DeviceSignalFxoPrimAnswer			DeviceSignalDialToneStart
//#define	DeviceSignalFxoIncomingSucess		DeviceSignalDialToneStop
#define	DeviceSignalFxoDialing				DeviceSignalLocalRingbackStart


#if 1
enum HardwareMsgType
{
	HardwareSignalType,
	HardwareAudioType,
	HardwareTimeoutType  /* for PBX virtual off-hook */
};

enum HardwareAudioRequestType
{
	AudioStart,
	AudioStop,
	AudioSuspend,
	AudioResume
};

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
	ActionNull
};

#define RGW_HOST_STRING_LEN (128)
#define RGW_MODE_LEN (128)

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

	//bool sendRingback;	
	AudioActionType actionType;
};


typedef union
{
	DeviceSignalType signal;
	struct HardwareAudioRequest request;
} HardwareMsgUnion;

#endif

/*
* PbxEvent destinate to DeviceManager Thread, 
* such as ISDN/PSTN/SIP/Console Manager thread, send by other thread 
*/
class PbxHwEvent : public PbxEvent
{
	public:
		PbxHwEvent();
		PbxHwEvent( const Sptr < Fifo < Sptr <PbxEvent> > > outputFifo );
		virtual ~PbxHwEvent();
		virtual const char* const name() const;

		void startTimer(Sptr <PbxEvent > newEvent, const int timerMs);

		//TODO move public data to protected area
		HardwareMsgType 			type;
		HardwareMsgUnion 		signalOrRequest;

		vector <string> 			dataList;
};
 
}

#endif

