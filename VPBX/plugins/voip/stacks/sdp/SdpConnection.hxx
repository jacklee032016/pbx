#ifndef _SDPCONNECTION_H
#define _SDPCONNECTION_H
/*
* $Log: SdpConnection.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:05:22  lizhijie
* no message
*
* $Id: SdpConnection.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include <vector>
#include <strstream>
//#include <assert.h>
#include <cassert>
#include "SdpExterns.hxx"

#include "cpLog.h"


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

/** Muticast address and the paramemter that goes with it
  * in the connection details.
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
        char* getAddress()
        {
            return address;
        }
        ///
        void setAddress(const char* addr)
        {
            strcpy(address, addr);
        }

        ///
        void encode (ostrstream& s);

    private:
        char address[MaxAddressLen];
        int ttl;
        int numAddress;
};


/** Connection details
  * For unicast address:
  *     c=<network type> <address type> <connection address>
  * For multicast address:
  *     c=IN IP4 <base multicast address>/<ttl>/<number of addresses>
*/
class SdpConnection
{
    public:
        ///
        SdpConnection();
        ///
        SdpConnection(string& str);
        ///
        SdpConnection( const SdpConnection& connection );
        ///
        void setUnicast(const char* addr)
        {
            strcpy(address, addr);
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
        string getUnicast() const
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

        /// Implement INVITE(HOLD)
        void setHold();      // set  <connection address> to "0.0.0.0"
        bool isHold();

    private:
        ///
        string networkTypeString();
        ///
        string addressTypeString();

        NetworkType networktype;

        AddressType addresstype;

        /// Unicast address
        char address[MaxAddressLen];

        SdpMulticast* multicast;
};

#endif
