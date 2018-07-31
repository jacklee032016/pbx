#ifndef RTCPRECEIVER_HXX
#define RTCPRECEIVER_HXX

/*
* $Log: RtcpReceiver.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:04:36  lizhijie
* no message
*
* $Id: RtcpReceiver.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "Rtcp.hxx"

#include <sys/types.h>
#include <map>

#include "UdpStack.hxx"

#include "rtpTypes.h"
#include "NetworkAddress.h"



/** Class to receive RTCP packets.
    @see RtpSession
 */
class RtcpReceiver
{
    public:
        /** Constructor creating incoming RTCP stack using port ranges
            @param port associated port
         **/
        RtcpReceiver (int localMinPort, int localMaxPort);
        /** Constructor creating incoming RTCP stack using specified port
            @param port associated port
         **/
        RtcpReceiver (int localPort);
        /** Constructor, using udp stack ptr **/
        RtcpReceiver (UdpStack * udp);

        /// consturctor init (don't call this function)
        void constructRtcpReceiver ();
        ///
        ~RtcpReceiver ();


        /** Receives an incoming RTCP packet
         *  @return: NULL no data recprocess
         **/
        RtcpPacket* getPacket ();

        /** Checks if an RTCP packet is valid
         *  @return 0 not valid, 1 valid
         **/
        int isValid (RtcpPacket* packet);

        /// reads compound RtcpPacket and calls apporiate read function
        int readRTCP (RtcpPacket* packet);

        /** searches inside packet for given type.
            @return NULL means type not found, else ptr to first found
         **/
        RtcpHeader* findRTCP (RtcpPacket* packet, RtcpType type);


        /**
         *  These functions will search inside packet for apporiate type
         *  @return -1 if type not found.  Otherwise will call related
         *  function to use packet and return 0.
         **/
        int readSR (RtcpPacket* packet);
        ///
        int readSDES (RtcpPacket* packet);
        ///
        int readBYE (RtcpPacket* packet);
        ///
        int readAPP (RtcpPacket* packet);

        /**
         *  These functions will search inside packet for apporiate type
         * Thses functions will read packet and store packet information.
         * Same result as calling, ie  readSR(findRTCP(p, RtcpTypeSR))
         **/
        void readSR (RtcpHeader* head);
        ///
        void readSDES (RtcpHeader* head);
        ///
        int readBYE (RtcpHeader* head);
        ///
        void readAPP (RtcpHeader* head);


        ///
        void printSR (RtcpSender* p);
        ///
        void printRR (RtcpReport* p);
        ///
        void addSDESItem (RtpSrc src, RtcpSDESItem* item);


        /// Adds receiver to source listing
        RtpTranInfo* addTranInfo (RtpSrc src, RtpReceiver* recv = NULL);
        int addTranFinal (RtpTranInfo* s);
        /** Remove receiver from  source listing
         *  @return 0 sucess, 1 not found
         **/
        int removeTranInfo (RtpSrc src, int flag = 0);
        /** Finds pointer to source structure
         *  Creates source struture if not found
         *  @return pointer to source structure
         **/
        RtpTranInfo* findTranInfo (RtpSrc src);

        /// Access specified souurce infomration
        RtpTranInfo* getTranInfoList (int index);

        /// Number of known sources
        int getTranInfoCount ();

        /// Port this receiver is receiving it signal
        int getPort ();
        /// Socket File Descriptor used for select()
        int getSocketFD ();

        /// get the ptr of my UdpStack
        UdpStack* getUdpStack()
        {
            return myStack;
        }

        /// get the data for latency
        int getAvgOneWayDelay()
        {
            return avgOneWayDelay;
        }
        int getAvgRoundTripDelay()
        {
            return avgRoundTripDelay;
        }


    private:
        /// list of known sources
        map < RtpSrc, RtpTranInfo* > tranInfoList;

        /// my UDP stack
        UdpStack* myStack;

        ///
        bool freeStack;

        ///
        int packetReceived;

        ///
        int accumOneWayDelay;
        int avgOneWayDelay;

        ///
        int accumRoundTripDelay;
        int avgRoundTripDelay;


};


#endif // RTCPRECEIVER_HXX
