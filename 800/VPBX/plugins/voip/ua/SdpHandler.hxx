#ifndef SDP_HANDLER_HXX_
#define SDP_HANDLER_HXX_

#include "SipMsg.hxx"
#include "Data.hxx"
#include "Sptr.hxx"
//#include "UaHardwareEvent.hxx"
#include "SipSdp.hxx"
#include "SystemInfo.hxx"


using Assist::SipMsg;
using Assist::SipSdp;

void addSdpToMsg( SipMsg& msg, Data rtpRate, int rtpPort );

/// returns true if a valid remote SDP was received, false if not
bool validRemoteSdp ( Sptr<SipSdp> remoteSdp );


/// return the RtpPacketSize of this sdp message
int getRtpPacketSize(SipSdp& sipSdp); // xxx should be const


/// return true if able to set RTP packet size, false if not
void setRtpPacketSize(SipSdp& sipSdp, int packetSize);


void
setHost(SipSdp& sipSdp, Data hostAddr);

void
setStandardSdp(SipSdp& sipSdp, Data hostAddr, int port);

void
setUlaw(SipSdp& sipSdp, int rate);

bool validUlaw(SipSdp& sdp);

void
setupMedia(SipSdp& sipSdp, Data hostAddr, int port);

void
setAvt(SipSdp& sipSdp, int payloadType);

bool checkType(SipSdp& sdp, int& realPayloadType, Data eventName);

void
doAnswerStuff(SipSdp& local, Sptr<SipSdp> remote, Data hostAddr, int port);


/*
void setAudioRequest( struct Assist::HardwareAudioRequest* request,
                      SipSdp& remoteSdp,
                      SipSdp& localSdp );
*/

//modified by lijie
//void sendRemoteRingback(SipSdp& remoteSdp);
void sendRemoteRingback(SipSdp& remoteSdp, int deviceId);

//added by lijie
void stopAudio( int deviceId );

#endif
