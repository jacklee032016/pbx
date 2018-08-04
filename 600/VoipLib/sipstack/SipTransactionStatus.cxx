/*
 * $Id: SipTransactionStatus.cxx,v 1.2 2007/03/01 20:04:32 lizhijie Exp $
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
