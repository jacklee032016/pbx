#ifndef RTPTOOLS_H
#define RTPTOOLS_H

/*
 * $Id: rtpTools.hxx,v 1.2 2007/03/01 20:04:27 lizhijie Exp $
 */


#include "rtpTypes.h"


u_int32_t generate32 ();
RtpSrc generateSRC();
void printBits (char* packetData, int len);
void printInt (int x);

bool RtpSeqGreater (RtpSeqNumber, RtpSeqNumber);
bool RtpTimeGreater (RtpTime, RtpTime);

#endif // RTPOOLS_H
