/*
 * $Id: SipReceiveStatus.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
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
