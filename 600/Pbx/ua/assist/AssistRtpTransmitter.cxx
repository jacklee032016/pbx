
#include <AssistRtpTransmitter.hxx>

using namespace Vocal;

AssistRtpTransmitter::AssistRtpTransmitter(RtpSession *audioStack)
	:myAudioStack(audioStack)
{
	assert(myAudioStack);
}


AssistRtpTransmitter::~AssistRtpTransmitter()
{
}

int AssistRtpTransmitter::transmitRaw(char *buf, int cc)
{
		return myAudioStack->transmitRaw (buf, cc);
		
}

