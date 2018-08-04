/*
* $Log: SipReceiveStatus.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipReceiveStatus.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/
#include "SipReceiveStatus.hxx"
#include "cpLog.h"

SipReceiveStatus::SipReceiveStatus()
        :
        tcpConnection(0),
        tcpValid(false),
        sipmsg(0),
        sweepMsg(false)
{}



SipReceiveStatus::SipReceiveStatus( const SipReceiveStatus& other)
        :
        tcpConnection(other.tcpConnection),
        tcpValid(other.tcpValid),
        sipmsg(other.sipmsg),
	sweepMsg(other.sweepMsg)
{}


SipReceiveStatus& 
SipReceiveStatus::operator = (const SipReceiveStatus& other) const
{
    if (&other != this)
    {
       tcpConnection = other.tcpConnection;
       tcpValid = other.tcpValid;
       sipmsg = other.sipmsg;
       sweepMsg = other.sweepMsg;
        
    }
    return (*this);
}
