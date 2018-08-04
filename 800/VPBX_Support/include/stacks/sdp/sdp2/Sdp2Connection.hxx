#ifndef SDP2CONNECTION_HXX_
#define SDP2CONNECTION_HXX_
/*
* $Log: Sdp2Connection.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: Sdp2Connection.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include <vector>
#include <strstream>
#include <cassert>

#include "Sdp2Externs.hxx"
#include "cpLog.h"
#include "Data.hxx"


/** Infrastructure common to VOCAL.
 */
namespace Assist 
{


/** Infrastructure in VOCAL related to logging.<br><br>
 */
namespace SDP
{


///
enum TransportType
{
    TransportTypeUnknown,
    TransportTypeRTP,
    TransportTypeUDP
};

///
enum AddressType
{
    AddressTypeUnknown,
    AddressTypeIPV4,
    AddressTypeIPV6
};

///
enum NetworkType
{
    NetworkTypeUnknown,
    NetworkTypeInternet
};


const int MulticastTtlValMin = 0;
const int MulticastTtlValMax = 255;
const int MaxAddressLen = 128;

/** 
    A Multicast address and the parameter that goes with it in the
    SdpConnection.

  */
class SdpMulticast
{
    public:
        ///
        SdpMulticast();
        ///
        SdpMulticast(const char* addr, int ttlval = 0, int numAddr = 1)
        {

            setAddress(addr);
            setTtl(ttlval);
            setnumAddr(numAddr);
        }
        ///
        void setTtl(int ttlval)
        {
            if ( (ttlval < MulticastTtlValMin) ||
                    (ttlval > MulticastTtlValMax) )
                cpLog(LOG_ERR, "ttl out of range");
            else
                ttl = ttlval;
        }
        ///
        int getTtl()
        {
            return ttl;
        }
        ///
        void setnumAddr(int numAddr)
        {
            numAddress = numAddr;
        }
        ///
        int getnumAddr()
        {
            return numAddress;
        }
        ///
        Data getAddress()
        {
            return address;
        }
        ///
        void setAddress(Data addr)
        {
            address = addr;
        }

        ///
        void encode (ostrstream& s);

    private:
        Data address;
        int ttl;
        int numAddress;
};


/** 
    SDP Connection details.
    For a unicast address:<p>
    <pre>
    c=&lt;network type> &lt;address type> &lt;connection address>
    </pre>
    <p>
    For a multicast address:<p>
    <pre>
    c=IN IP4 &lt;base multicast address>/&lt;ttl>/&lt;number of addresses>
    </pre>
    <p>
    A complete example of usage is available in the SdpSession class
    description.<p>

    @see SdpSession
*/
class SdpConnection
{
    public:
        ///
        SdpConnection();
        ///
        SdpConnection(Data str);
        ///
        SdpConnection( const SdpConnection& connection );
        ///
        void setUnicast(Data addr)
        {
            address = addr;
        }
        ///
        void setMulticast(const char* addr, int ttl, int numAddr = 1)
        {
            if (!multicast)
            {
                multicast = new SdpMulticast(addr, ttl, numAddr);
            }
            else
            {
                multicast->setAddress(addr);
                multicast->setTtl(ttl);
                multicast->setnumAddr(numAddr);
            }
        }
        ///
        SdpMulticast* getMulticast() const
        {
            return multicast;
        }
        ///
        Data getUnicast() const
        {
            return address;
        }
        ///
        void setNetworkType(NetworkType networkType)
        {
            networktype = networkType;
        }
        ///
        NetworkType getNetworkType() const
        {
            return networktype;
        }
        ///
        void setAddressType(AddressType addressType)
        {
            addresstype = addressType;
        }
        ///
        AddressType getAddressType() const
        {
            return addresstype;
        }
        ///
        SdpConnection& operator=(const SdpConnection& connection);
        ///
        ~SdpConnection()
        {
            if (multicast)
                delete multicast;
        }

        ///
        void encode (ostrstream& s);

        Data encode() const;

        /**Implements INVITE(HOLD)
         * set  <connection address> to "0.0.0.0"
         */
        void setHold();      
        bool isHold();

    private:
        ///
        Data networkTypeString() const;
        ///
        Data addressTypeString() const;

        NetworkType networktype;

        AddressType addresstype;

        /// Unicast address
        Data address;

        SdpMulticast* multicast;
};

}

}
#endif
