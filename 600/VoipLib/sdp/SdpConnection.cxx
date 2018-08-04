/*
 * $Id: SdpConnection.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */

#include "global.h"
#include <string>
#include "support.hxx"

#include "SdpExceptions.hxx"
#include "SdpConnection.hxx"

/************************ SdpConnection class methods *************************/

///
SdpConnection::SdpConnection ()
{
    addresstype = AddressTypeIPV4;
    networktype = NetworkTypeInternet;
    strcpy (address, "0.0.0.0");
    multicast = 0;
}    // SdpConnection::SdpConnection


///
SdpConnection::SdpConnection (string& str)
{
    //initialize multicast.
    multicast = 0;
    //str.erase(0,2);

    //split based on '/'

    // use split instead of sub_split.
    // -ctam

    split_t connectionList(split(str, "/"));

    if (connectionList.size() < 1)
    {
        // not enough parameters
        cpLog(LOG_ERR, "SdpConnection: parameter count < 1 on connection list");
        throw SdpExceptions(PARAM_NUMERR);
    }
    else // >=1
    {
        if (connectionList.size() > 1)
        {
            cpDebug(LOG_DEBUG_STACK, "connectionList size is %d", connectionList.size() );
            cpDebug(LOG_DEBUG_STACK, "Multi-cast address in %s", str.c_str() );
            //set the multicast specific parts.
            //create the multicast details.
            multicast = new SdpMulticast;
            assert(multicast);
            multicast->setTtl(atoi(connectionList[1].c_str()) );

        }
        if (connectionList.size() > 2)
        {
            assert(multicast);
            multicast->setnumAddr(atoi(connectionList[2].c_str() ));
        }
        //sub parms: get the address specific.
        //split again.
        //const char* str = connectionList[0].c_str();
        string str2 = connectionList[0].c_str();
        string addressSpec(str2);


        // take out sub_split.  This may be causing corruption in the sdp.
        // -ctam

        split_t addrSpec(split(addressSpec, " "));

        // Use nTypeStr to make a copy of the addrSpec.
        string nTypeStr = addrSpec[0].c_str();

        if (strcmp(nTypeStr.c_str(), SdpNetworkTypeIN) == 0)
        {
            networktype = NetworkTypeInternet;
        }
        else
        {
            cpLog(LOG_ERR, "addressSpec: {%s}", addressSpec.c_str());
            // bad!

            // -ctam
            printf("***  nTypeStr.c_str()= <%s>, SdpNetworkTypeIN = <%s> ***\n",
                   nTypeStr.c_str(), SdpNetworkTypeIN );

            cpLog( LOG_ERR, "SdpConnection: unknown transport type");
            throw SdpExceptions(UNKNOWN_NETTYPE);
        }
        if (addrSpec[1] == SdpAddressTypeIP4)
        {
            addresstype = AddressTypeIPV4;
        }
        else if (addrSpec[1] == SdpAddressTypeIP6)
        {
            addresstype = AddressTypeIPV6;
        }
        else
        {
            cpLog(LOG_ERR, "SdpConnection: unknown address type");
            throw SdpExceptions(UNKNOWN_ADDRTYPE);
        }
        if (multicast)
        {
            multicast->setAddress(addrSpec[2].c_str());
        }
        else
        {
            strcpy(address, addrSpec[2].c_str());
        }
    }
}


///
SdpConnection&
SdpConnection::operator=(const SdpConnection& connection)
{
    cpDebug(LOG_DEBUG_STACK, "SdpConnection operator=" );

    networktype = connection.getNetworkType();
    addresstype = connection.getAddressType();

    SdpMulticast* mult = connection.getMulticast();

    if (mult) //multicast address present.
    {
        cpDebug(LOG_DEBUG_STACK, "Multi-cast address" );
        if (!multicast)
        {
            multicast = new SdpMulticast;
            assert(multicast);
        }
        (*multicast) = (*mult);
    }
    else
    {
        //if previously multicast is defined, delete that.
        if (multicast)
        {
            cpDebug(LOG_DEBUG_STACK, "Delete multi-cast address" );
            delete multicast;
        }
        multicast = 0;
        //assign the unicast address.
        strcpy(address, connection.getUnicast().c_str());
    }
    return *(this);
}


/// Copy constructor
SdpConnection::SdpConnection( const SdpConnection& connection )
{
    cpDebug(LOG_DEBUG_STACK, "SdpConnection copy constructor" );

    networktype = connection.getNetworkType();
    addresstype = connection.getAddressType();
    strcpy(address, connection.getUnicast().c_str());

    SdpMulticast* mult = connection.getMulticast();

    if ( mult )
    {
        cpDebug(LOG_DEBUG_STACK, "Multi-cast address" );
        multicast = new SdpMulticast;
        assert(multicast);
        *multicast = *mult;
    }
    else
    {
        multicast = 0;
    }
}


///
string
SdpConnection::networkTypeString ()
{
    string s;

    switch (networktype)
    {
        case NetworkTypeInternet:
        {
            s = SdpNetworkTypeIN;
            break;
        }
        default:
        {
            //TODO Throw some exception
            break;
        }
    }
    return s;
}    // SdpConnection::networkTypeString


///
string
SdpConnection::addressTypeString ()
{
    string s;

    switch (addresstype)
    {
        case AddressTypeIPV4:
        {
            s = SdpAddressTypeIP4;
            break;
        }
        case AddressTypeIPV6:
        {
            s = SdpAddressTypeIP6;
            break;
        }
        default:
        {
            //TODO Throw some exception
            break;
        }
    }
    return s;
}    // SdpConnection::addressTypeString


///
void
SdpConnection::encode (ostrstream& s)
{
    s << "c=" << networkTypeString()
    << ' '
    << addressTypeString()
    << ' ';
    if (multicast)
    {
        multicast->encode (s);
    }
    else
    {
        s << address;
    }
    s << "\r\n";
}    // SdpConnection::encode

///
void
SdpConnection::setHold ()
{
    if ( !multicast )
    {
        // We are copying a fixed string, hence no need for
        // those init buffer & copy with strncpy(..) overhead
        strcpy(address, "0.0.0.0");
    }
    else
    {
        multicast->setAddress("0.0.0.0");
    }
}

///
bool
SdpConnection::isHold ()
{
    string addr;

    if ( !multicast )
    {
        addr = getUnicast();
    }
    else
    {
        addr = multicast->getAddress();
    }

    return (addr == "0.0.0.0");
}

/************************** SdpMulticast class methods *************************/

SdpMulticast::SdpMulticast() :
        numAddress( 0 )
{}

///

void
SdpMulticast::encode (ostrstream& s)
{
    s << address << '/' << ttl;
    if (numAddress > 0)
    {
        s << '/' << numAddress;
    }
}    // SdpMulticast::encode
