#ifndef RTPCODEC_H
#define RTPCODEC_H
/*
* $Log: rtpCodec.h,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: rtpCodec.h,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "Rtp.hxx"
#include "rtpTypes.h"

struct RtpCodecInfo
{
    RtpPayloadType payloadType;
    char codecString[32];
    int length;
    unsigned char silence[480];
};

extern const int numRtpCodecInfo;
extern RtpCodecInfo rtpCodecInfo[];

//unsigned char linear2ulaw( int pcm_val );
//int ulaw2linear( unsigned char   u_val);
RtpPacket* convertRtpPacketCodec (RtpPayloadType toType, RtpPacket* packet);
int convertCodec(RtpPayloadType fromType, RtpPayloadType toType,
                 char* in_data, char* out_data, int len);
void rtp_htonl(RtpPacket* p);
void rtp_ntohl(RtpPacket* p);
char* findSilenceCodec (RtpPayloadType type, int len);
char* findSilenceCodecString (char* codecStringInput, int len);
void createSilenceCodec (char* buffer, RtpPayloadType type, int len);
void setNetworkFormatCodec();

#endif
