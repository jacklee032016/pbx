#ifndef RTPTOOLS_H
#define RTPTOOLS_H
/*
* $Log: rtpTools.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:04:36  lizhijie
* no message
*
* $Id: rtpTools.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "rtpTypes.h"


u_int32_t generate32 ();
RtpSrc generateSRC();
void printBits (char* packetData, int len);
void printInt (int x);

bool RtpSeqGreater (RtpSeqNumber, RtpSeqNumber);
bool RtpTimeGreater (RtpTime, RtpTime);

#endif // RTPOOLS_H
