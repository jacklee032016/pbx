/*
* $Id: PstnAudioChannel.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#ifndef __PSTN_AUDIO_CHANNEL_HXX__
#define __PSTN_AUDIO_CHANNEL_HXX__

#include "AudioChannel.hxx"

namespace Assist
{
class PstnCtrlDevice;
class EndPoint;

#define VOICE_RATE			160

class PstnAudioChannel : public AudioChannel 
{
	public:
		PstnAudioChannel( PstnCtrlDevice *_parent, int fd );
		~PstnAudioChannel();	
		void activate( );
		void deactivate();		

		int ReadVData(char* buffer);
		int WriteVData(char* buffer, int length);

		virtual void processAudio();
		
	protected:

	private:
		VMutex 		myMutex;

};

}
#endif

