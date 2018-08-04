#ifndef __VOIP_AUDIO_CHANNEL_HXX__
#define __VOIP_AUDIO_CHANNEL_HXX__
/*
* $Id: VoipAudioChannel.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "vpbx_globals.h"
#include "Sptr.hxx"
#include "AudioChannel.hxx"
#include "PbxHwEvent.hxx"
#include "RtpSession.hxx"
//#include "pbx_globals.h"

namespace Assist
{
class VoipCtrlDevice;

#ifndef VOICE_RATE
#define VOICE_RATE			160
#endif 

typedef struct 
{
	unsigned char digits[16];
	int in_pos;
	int out_pos;
}as_pbx_dtmf_buffer;

typedef enum
{
	as_pbx_dtmf_invalide = -1,
	as_pbx_dtmf_rfc2883 = 0,
	as_pbx_dtmf_audio  = 1	
}as_pbx_dtmf_mode_t;


class VoipAudioChannel : public AudioChannel 
{
	public:
		VoipAudioChannel( VoipCtrlDevice *_parent, int fd );
		~VoipAudioChannel();	
	public:

		void activate( );
		void deactivate();		

		int ReadVData(char* buffer);
		int WriteVData(char* buffer, int length);

		void transmitRTPDTMF(char digit);
		void recvRTPDTMF( int digit, int duration );
		int audioStart (const HardwareAudioRequest& request );
		int audioStop();
		int audioSuspend ();
		int audioResume (const HardwareAudioRequest& request);

		void releaseRtpPort();
		int reserveRtpPort(int localMin, int localMax, int jitterNew);
		int getRtpPort();
	
		
	protected:
		//bool emitDtmfSignal(char signal);
		int openDeviceFile();
		int getSendBackTone(char *ringData, int size);

	private:
		VMutex 						audioMutex;
		VMutex 						actionMutex;

		struct timeval                          	prevTime;
		int 							ringbackFd;
		int 							greetingFd;
		int 							busyFd;
		int 							callwaitingFd;
		int 							dialFd;
		AudioActionType 				actionType;
		as_pbx_dtmf_buffer  			dtmf_buf;
		as_pbx_dtmf_mode_t  			pbxDtmfMode;
		bool 						audioActive;      // true if two way speech is enabled
		bool 						audioHalfActive;  // true if one way speech is enabled

		RtpSession					*audioStack;
		//AssistRtpTransmitter *as_tran;
		RtpPacket					*inRtpPkt;
		DTMFInterface					*_DTMFInterface;		
		char 						outBufferPkt[480];

		int							currentDigit;				/* current digit transmit by RFC 2833 */

		RtpTime 						mySeedRtpTime;			/* RTP time of previous packet*/
		RtpTime 						prevRtpTime;
		RtpPacket					*outRtpPkt;
};

}
#endif

