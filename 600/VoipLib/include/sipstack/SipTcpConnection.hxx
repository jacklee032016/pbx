#ifndef SIP_TCP_CONNECTION_HXX_
#define SIP_TCP_CONNECTION_HXX_
/*
 * $Id: SipTcpConnection.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include "Fifo.h"
#include "SipMsg.hxx"
#include "Sptr.hxx"
#include "TransceiverSymbols.hxx"

#include "SipTransactionLevels.hxx"
#include "SipTransactionGC.hxx"

namespace Vocal
{
    
class SipTcpConnection_impl_;
struct SipMsgContainer;

class SipTcpConnection
{
    public:
        ///
        SipTcpConnection(Fifo <SipMsgContainer*> * fifo, int port = SIP_PORT);
        ///
        virtual ~SipTcpConnection();
        ///
        void send(SipMsgContainer* msg, const Data& host="",
                             const Data& port="");

    private:
        ///
        SipTcpConnection_impl_* impl_;
};

int isFullMsg(const string& str);
 
} // namespace Vocal

#endif
