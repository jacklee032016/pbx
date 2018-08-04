#ifndef SipTransactionStatus_HXX
#define SipTransactionStatus_HXX
/*
* $Log: SipTransactionStatus.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipTransactionStatus.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "SipReceive.hxx"
#include "Connection.hxx"
#include <Sptr.hxx>

namespace Assist
{
    
class SipMsg;


/// data container for TransactionStatus header - only used inside SIP stack
class SipTransactionStatus
{
    public:
        SipTransactionStatus();
        SipTransactionStatus( const SipTransactionStatus& other);

        ~SipTransactionStatus();
        ///
        bool getDeleteMsg();
        ///
        void setDeleteMsg(const bool& pleaseDelete);
        ///
        Sptr < Connection > tcpConnection;
        ///
        bool tcpValid;
        ///
        int nextRetransmissionDelay;
        ///
        int numberOfRetries;
        ///
        Sptr < SipMsg > msg;
        ///
        bool deleteMsg;
        ///
        bool retransmit;
        ///
        bool isAsync;
        ///
        Sptr < SipReceive > msgChain;    //contains the current, and prev msg.
        ///
        bool sweepDeleteMsg;
};

 
}

#endif
