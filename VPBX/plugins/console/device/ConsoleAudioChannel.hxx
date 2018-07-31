/*
* $Id: ConsoleAudioChannel.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#ifndef __CONSOLE_AUDIO_CHANNEL_HXX__
#define __CONSOLE_AUDIO_CHANNEL_HXX__

#include "vpbx_globals.h"
#include "Sptr.hxx"

#include "AudioChannel.hxx"
#include "console.hxx"

#include "assist_lib.h"

namespace Assist
{
class ConsoleCtrlDevice;

class ConsoleAudioChannel : public AudioChannel 
{
	public:
		ConsoleAudioChannel( CtrlDevice *_parent, console_dev_t  isMaster);
		~ConsoleAudioChannel();

		void activate( );
		void deactivate();
		
		virtual int ReadVData(char* buffer);
		virtual int WriteVData(char* buffer, int length);

		Sptr <ConsoleAudioChannel> getSlave();
		void setSlave(Sptr <ConsoleAudioChannel> slave);
		
		void setAudioState( console_audio_state_t audioState);

		//void stopDialTone();

		virtual void emitDtmfSignal(char dtmf);
		virtual void stopDialTone();

		bool								isDecoding;	/* decoding thread is running */
		as_dsp_t							*myDsp;	/* refer to DSP in ConsoleEndPoint */

		int								dtmfBufCount;
		int 								dtmfBufSize;
		
		unsigned char						*dtmfBufs;					/* size id dtmfBufSize*dtmfBufCount */

		int								dtmfBufInPos;
		int								dtmfBufInIndex;				/* which dtmfBuf is used to RX data */
		int								dtmfBufOutIndex;			/* which dtmfBuf is used to detect DTMF phone number */

	protected:

	
private:
		int 	readGreeting(char *buffer);

		int 								myPeerFd;

		VMutex 							myMutex;

		console_audio_state_t				myCAudioState;

		Sptr <ConsoleAudioChannel>		mySlave;

		console_dev_t						myDevType;

		int 								myGreetingSize;
		int 								myGreetingIndex;
		char								*myGreetingData;

		struct timeval                          		prevTime;		
};

}
#endif

