#ifndef __ASSIST_RTP_TRANSMITTER_H__
#define __ASSIST_RTP_TRANSMITTER_H__

#include <VMutex.h>
#include "VException.hxx"

#include <Sptr.hxx>
#include <SipProxyEvent.hxx>
#include <SipCallId.hxx>

#include "RtpSession.hxx"
#include "ResGwDevice.hxx"
#include "VMutex.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "pbx_globals.h"

namespace Vocal
{

class  AssistRtpTransmitter
{
	public:

		AssistRtpTransmitter(RtpSession *audioStack) ;
		
		virtual ~AssistRtpTransmitter();

		virtual int transmitRaw(char *buf, int cc);
			
	protected:

		RtpSession *myAudioStack;
		
	private:

};

}
#endif


