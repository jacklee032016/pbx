#ifndef SipTransactionStatus_HXX
#define SipTransactionStatus_HXX
/*
* $Log: SipTransactionStatus.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipTransactionStatus.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
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
