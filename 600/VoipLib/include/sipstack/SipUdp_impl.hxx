#ifndef SipUdp_impl_HXX
#define SipUdp_impl_HXX
/*
 * $Id: SipUdp_impl.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include "global.h"
#include "Sptr.hxx"
#include "SipTransactionStatus.hxx"
#include "VThread.hxx"
#include "UdpStack.hxx"
#include "Fifo.h"
#include "TransceiverSymbols.hxx"  
#include "RetransmitContents.hxx"

#ifdef USE_VFILTER
#include "VFilter.hxx"  
#endif

#include "SipTransactionLevels.hxx"
#include "SipTransactionGC.hxx"


namespace Vocal
{
    
///
class SipUdp_impl
{
    public:

    ///
        SipUdp_impl(Fifo<SipMsgContainer*>* fifo ,int port = SIP_PORT);
        ///
        ~SipUdp_impl();
        ///
        int udpSend(SipMsgContainer* msg);
        ///
        void send(SipMsgContainer* msg, const Data& host="",
		  const Data& port="5060");
        ///
        static void reTransOff();
        ///
        static void reTransOn();
        ///
        static void setRetransTime(int initial = retransmitTimeInitial , int max = retransmitTimeMax );
        ///
        void setRandomLosePercent(int percent);
        ///
        void printSize();
        ///
        Data getDetails() const;
    private:
        ///
        static bool Udpretransmitoff;
        ///
        static int  Udpretransmitimeinitial;
        ///
        static int Udpretransmittimemax;
        ///
        int randomLosePercent;
        ///
        UdpStack udpStack;
        ///
        Fifo<SipMsgContainer * >* recFifo;
        ///
        Fifo<RetransmitContents *> sendFifo;
        /// 
        VThread sendThread;
        ///
        VThread receiveThread;
        ///
        bool shutdown;                                       
        ///
        static void* sendThreadWrapper(void *p);
        /// 
        static void* rcvThreadWrapper(void *p);
        ///    
        void* receiveMain();
        ///    
        void* sendMain();
        ///        
        void getHostPort(Sptr<SipMsg> sipMessage, Data* host, int* port);
        ///
        SipUdp_impl& operator =(const SipUdp_impl& src) const;
};
 
} // namespace Vocal

#endif
