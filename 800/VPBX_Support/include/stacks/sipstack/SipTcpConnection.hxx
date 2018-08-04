#ifndef SIP_TCP_CONNECTION_HXX_
#define SIP_TCP_CONNECTION_HXX_
/*
* $Log: SipTcpConnection.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipTcpConnection.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "Fifo.h"
#include "SipMsg.hxx"
#include "Sptr.hxx"
#include "TransceiverSymbols.hxx"

#include "SipTransactionLevels.hxx"
#include "SipTransactionGC.hxx"

namespace Assist
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
 
} 

#endif
