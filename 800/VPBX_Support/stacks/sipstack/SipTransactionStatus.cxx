/*
* $Log: SipTransactionStatus.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:10  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipTransactionStatus.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "SipTransactionStatus.hxx"
#include "cpLog.h"
#include "SipMsg.hxx"

SipTransactionStatus::SipTransactionStatus()
        :
        tcpConnection(0),
        tcpValid(false),
        nextRetransmissionDelay(100),
        numberOfRetries(0),
        msg(0),
        deleteMsg(false),
        retransmit(true),
        isAsync(false),
        msgChain(0),
        sweepDeleteMsg(false)

{
    msgChain = new SipReceive;
}

SipTransactionStatus::~SipTransactionStatus()
{
    cpDebug(LOG_DEBUG_STACK, "~SipTransactionStatus()");
}


void SipTransactionStatus::setDeleteMsg(const bool& pleaseDelete )
{
    cpDebug(LOG_DEBUG_STACK, "setting delete msg to: %d", pleaseDelete);
    deleteMsg = pleaseDelete;
}
bool SipTransactionStatus::getDeleteMsg()
{
    return deleteMsg;
}


SipTransactionStatus::SipTransactionStatus( const SipTransactionStatus& other)
        :
        tcpConnection(other.tcpConnection),
        tcpValid(other.tcpValid),
        nextRetransmissionDelay(other.nextRetransmissionDelay),
        numberOfRetries(other.numberOfRetries),
        msg(other.msg),
        deleteMsg(other.deleteMsg),
        retransmit(other.retransmit),
        isAsync(other.isAsync),
        msgChain(other.msgChain)
{
}
