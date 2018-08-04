#ifndef SIP_UDP_CONNECTION_HXX
#define SIP_UDP_CONNECTION_HXX
/*
* $Log: SipUdpConnection.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipUdpConnection.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "Sptr.hxx"
#include "SipMsg.hxx"
#include "Fifo.h"
#include "SipTransactionId.hxx"
#include "TransceiverSymbols.hxx"

#include "SipTransactionLevels.hxx"

namespace Assist
{
    
class SipUdp_impl;
struct SipMsgContainer;

///
class SipUdpConnection
{
    public:
        ///
        SipUdpConnection(Fifo <SipMsgContainer *> *fifo, int port = SIP_PORT);
        ///
        ~SipUdpConnection();
        ///
        void send(SipMsgContainer* msg, const Data& host="",
                       const Data& port="5060");

        /// ???????
        //void  receive();
        ///
        static void reTransOn();
        ///
        static void reTransOff();
        ///
  static void setRetransTime(int initial = retransmitTimeInitial , int max = retransmitTimeMax /* default values from TransceiverSymbols.hxx */);
        ///
        void setRandomLosePercent(int percent);
        ///
        void printSize() const;
        ///
        Data getDetails() const;
    private:
        ///
        SipUdpConnection&
        operator = (const SipUdpConnection& src) const;
        SipUdp_impl* impl_;
};
 
} 

#endif
