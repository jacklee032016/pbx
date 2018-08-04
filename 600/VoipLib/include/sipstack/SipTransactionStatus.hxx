#ifndef SipTransactionStatus_HXX
#define SipTransactionStatus_HXX
/*
 * $Id: SipTransactionStatus.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */


#include "SipReceive.hxx"
#include "Connection.hxx"
#include <Sptr.hxx>

namespace Vocal
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

 
} // namespace Vocal

#endif
