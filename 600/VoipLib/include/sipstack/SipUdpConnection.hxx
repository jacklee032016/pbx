#ifndef SIP_UDP_CONNECTION_HXX
#define SIP_UDP_CONNECTION_HXX
/*
 * $Id: SipUdpConnection.hxx,v 1.3 2007/06/05 11:33:25 lizhijie Exp $
 */

#include "Sptr.hxx"
#include "SipMsg.hxx"
#include "Fifo.h"
#include "SipTransactionId.hxx"
#include "TransceiverSymbols.hxx"

#include "SipTransactionLevels.hxx"

namespace Vocal
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
		void send(SipMsgContainer* msg, const Data& host="", const Data& port="5060");

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
 
} // namespace Vocal

#endif
