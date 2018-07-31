/*
* $Id: IsdnAudioChannel.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#ifndef __ISDN_AUDIO_CHANNEL_HXX__
#define __ISDN_AUDIO_CHANNEL_HXX__

#include "vpbx_globals.h"
#include "Sptr.hxx"

#include "AudioChannel.hxx"

namespace Assist
{

#define B_RAWDEV							128//AS_ISDN_RAW_DEVICE

#define B_CHAN_PERPORT						2

#define ANY_CHANNEL 						0xff /* IE attribut for 'any channel' */


class IsdnCtrlDevice;

typedef enum
{
	B_TYPE_RAW			=	0,
	B_TYPE_MAIN			
}b_type_t;

class IsdnAudioChannel : public AudioChannel 
{
	public:
		IsdnAudioChannel( IsdnCtrlDevice *_parent, int _bChanId);//, int stackId );
		~IsdnAudioChannel();

		void activate( );
		void deactivate();
		
#if WITH_DEBUG
		virtual const char *debugInfo();
#endif

		int getChanId();
		
		virtual int ReadVData(char* buffer);
		virtual int WriteVData(char* buffer, int length);

		virtual void setEndPoint(Sptr <EndPoint> ep);

	protected:

	private:
		int 			myBChIndex;		/* CHAN_ID_B1_CHANNEL or CHAN_ID_B2_CHANNEL ,private to B Channel of ISDN BRI */

		int 			myPeerFd;

//		VMutex 		myMutex;
};

}
#endif

