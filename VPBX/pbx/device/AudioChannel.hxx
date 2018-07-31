#ifndef __AUDIO_CHANNEL_HXX__
#define __AUDIO_CHANNEL_HXX__

/*
* $Id: AudioChannel.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "Sptr.hxx"

#include "vpbx_globals.h"

namespace Assist
{

class EndPoint;
class CtrlDevice;

#define AS_PBX_DTMF_BUFFER_LENGTH		16

/* 800 bytes, 100ms */
#define AUDIO_BUFFER_SIZE					320//1024	//2096

typedef enum
{
	AUDIO_STATE_IDLE			=	0,
	AUDIO_STATE_ACTIVATING	=	1,		/* ISDN B Channel has been allocated, but not activated for communication */
	AUDIO_STATE_ACTIVE		=	2,
	AUDIO_STATE_DEACTIVATING	=	3
}audio_state_t;

typedef enum
{
	PEER_NO			=	0,
	PEER_MASTER		=	1,
	PEER_SLAVE			=	2
}peer_state_t;

class AudioChannel;

class AudioChannel
{
	public:
		AudioChannel( CtrlDevice *_parent, const char *_name);
		virtual ~AudioChannel();

		/* used in ISDN mainly */
		virtual int hardwareMain(void *msg)
		{
			return 0;
		};
		
		virtual void activate( );
		virtual void deactivate();
		
		virtual int dspRxTx();

		audio_state_t getState();
		void setState(audio_state_t  _audioState);

#if WITH_DEBUG
		virtual const char *debugInfo() ;
#endif

		const char* const getName() const
		{
			return myName;
		};

		void setAudioPeer( const Sptr<AudioChannel> audioPeer );
		Sptr<AudioChannel> getAudioPeer() const { return myPeer ;}
		void releaseAudioPeer( );
		
		virtual void processAudio();


		void setIndex(int _index);

		int getIndex()
		{
			return myIndex;
		};

		virtual Sptr <EndPoint> getEndPoint();

		virtual void setEndPoint(Sptr <EndPoint> ep);

		Sptr <CtrlDevice>	getParent();

		virtual int ReadVData(char* buffer);
		virtual int WriteVData(char* buffer, int length);

		void setPlayBusyFlag(bool newFlag){ playBusy = newFlag;}
		bool getPlayBusyFlag() { return playBusy; }
		/**
		* Receive RFC2833 style numbers
		*/
		virtual void emitDtmfSignal(char dtmf){};

		/**
		* 
		*/
		virtual void stopDialTone(){};

		bool isBuildProcess() const
		{
			return myBuildProcess;			
		}

		void setBuildProcess(bool state)
		{
			myBuildProcess = state;
		}

		bool canProcess();

	protected:

		Sptr <CtrlDevice>				myParent;
		Sptr <AudioChannel>			mySelf;
		
		int 							myFd;		/* fd for audio device, it is raw device for ISDN BRI */

		audio_state_t 					myState;
		
		int 							myIndex;		/* assigned in GatwayMgr, begin from 0 */
		char							myName[VPBX_NAME_LENGTH];


		VMutex 						myMutex;
		peer_state_t					myPeerState;
		Sptr <AudioChannel> 			myPeer;
		

		Sptr <EndPoint>				myEndPoint;
		
		bool							playBusy;
		bool                                      	supRFC2833;

		bool                                       	myBuildProcess;
};

}

#endif

