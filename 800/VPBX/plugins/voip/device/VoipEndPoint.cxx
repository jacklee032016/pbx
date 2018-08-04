/*
* $Id: VoipEndPoint.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "CtrlDevice.hxx"
#include "CommonMsg.hxx"
#include "CallMsgBuilder.hxx"
#include "CallEvent.hxx"
#include "CallState.hxx"
#include "SipEvent.hxx"
#include "SipContext.hxx"
#include "PbxAgent.hxx"

#include "VoipAudioChannel.hxx"
#include "VoipEndPoint.hxx"
using namespace Assist;


VoipEndPoint::VoipEndPoint(  CtrlDevice *_ctrlDev, Sptr <SipTransceiver> sipStack,bool callLegHistory  )
	: EndPoint( _ctrlDev ),
	mySipContext(0),
	mySipStack(sipStack),
	myCallLegHistory(callLegHistory)
{
	setState( findState(VOIP_EP_STATE_IDLE) );

	assert(mySipStack != 0);
	
	myconnected = false;
	sprintf(myName, "VOIP-EP");

	cpLog(LOG_DEBUG, "My phone number is: %s", myPhoneNumber);
}

VoipEndPoint::~VoipEndPoint(void)
{
	cpLog(LOG_DEBUG, "Destorying.....");
}

const char *VoipEndPoint::getName(void)
{
	return myName;  
}

int VoipEndPoint::audioStart (const HardwareAudioRequest& request)
{

//	int fd = -1;
	cpLog( LOG_DEBUG, ">> Establishing audio on device %s" , getName());
	cpLog( LOG_DEBUG, ">> Listening on port: %d", request.localPort );
	cpLog( LOG_DEBUG, ">> Sending to host  : %s", request.remoteHost );
	cpLog( LOG_DEBUG, ">> Sending to port  : %d", request.remotePort );
	cpLog( LOG_DEBUG, ">> RTP packet size  : %d ms", request.rtpPacketSize );

	Sptr <VoipAudioChannel> audio;
	audio.dynamicCast(getAudio());
	assert(audio!=0);
	return audio->audioStart( request);
}

int VoipEndPoint::audioStop()
{
	cpLog( LOG_DEBUG, "***   Stopping audio on device %s  ***" ,getName());

	setPhoneNumber( "");
	setPeerNumber( "");

	Sptr <VoipAudioChannel> audio;
	audio.dynamicCast(getAudio() );
	assert(audio!=0);
	return audio->audioStop();
	
}

int VoipEndPoint::audioSuspend ()
{
	Sptr <VoipAudioChannel> audio;
	audio.dynamicCast(getAudio() );
	assert(audio!=0);
	return audio->audioSuspend();
}

int VoipEndPoint::audioResume (const Assist::HardwareAudioRequest& request)
{
	Sptr <VoipAudioChannel> audio;
	audio.dynamicCast(getAudio() );
	assert(audio!=0);

	return audio->audioResume(request);
}


int VoipEndPoint::openDeviceFile()
{
	Sptr <AudioChannel> audio = getAudio();
	assert(audio != 0);
	
	return 0;
}

void VoipEndPoint::recvRTPDTMF( int digit, int duration )
{
	const char *phone_pad = {"0123456789*#"};
	cpLog(LOG_DEBUG, "DTMF digit %d(duration is %d) has been received from RTP stack on %s", digit, duration, getName());

#if 0
	if(pbxDtmfMode != as_pbx_dtmf_audio)
		pbxDtmfMode = as_pbx_dtmf_rfc2883;
	else
		return;
#endif

	if( digit>=0 && digit< 12 )
	{
		emitDtmfSignal(phone_pad[digit]);
	}
	else
	{
		cpLog(LOG_ERR, "Unrecognized DTMF key received from RTP stack");
		return;
	}
	return ;
}

bool VoipEndPoint::emitDtmfSignal(char signal)
{
	cpLog(LOG_DEBUG, "Just discard the DTMF signal from RTP on hardware device");
	return false;
}

void  VoipEndPoint::reportDeviceInfo(CallAction action, const string& wrstr )
{
	cpLog(LOG_DEBUG, "%s report Device Info %s", getName(), wrstr.c_str() );
}

void VoipEndPoint::reportSipEvent(Sptr <SipMsg> msg, Sptr <SipMsgQueue> sipRcv)
{
	SipEvent *sipEvent = new SipEvent();
	sipEvent->setSipMsg(msg);
	sipEvent->setSipMsgQueue(sipRcv);
	
	cpLog(LOG_DEBUG, "report event in VOIP EndPoint : %s", getName() );
	if(myCallLegHistory) 
		sipEvent->setCallLeg();

	assert( mySelf != 0 );
	sipEvent->setEndPoint( mySelf);
	
	PbxAgent::instance()->getGateway()->reportEvent( sipEvent);
	cpLog(LOG_DEBUG, "VOIP EndPoint::reportPEvent");

}

void VoipEndPoint::setSipCallId(SipCallId &sipCallId)
{
	mySipCallId = sipCallId;
}

SipCallId &VoipEndPoint::getSipCallId()
{
	return mySipCallId;
}

void VoipEndPoint::setSipContext(Sptr <SipContext> context )
{
	mySipContext = context;
}

Sptr <SipContext> VoipEndPoint::getSipContext()
{
	return mySipContext;
}

Sptr <SipTransceiver> VoipEndPoint::getSipStack()
{
	return mySipStack;
}

int VoipEndPoint::getRtpPort()
{
	Sptr <VoipAudioChannel> vAudio;
	vAudio.dynamicCast(getAudio() );
	assert(vAudio!=0);

	return vAudio->getRtpPort();
}

void VoipEndPoint::reset()
{
	EndPoint::reset();

//	setSipCallId(0);
	setSipContext(0);
}

