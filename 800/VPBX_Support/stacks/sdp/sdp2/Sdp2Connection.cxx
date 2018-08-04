/*
* $Log: Sdp2Connection.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: Sdp2Connection.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include <list>
#include <deque>

#include "global.h"
#include "Data.hxx"
#include "support.hxx"

#include "Sdp2Exceptions.hxx"
#include "Sdp2Connection.hxx"


using Assist::SDP::SdpConnection;
using Assist::SDP::SdpMulticast;


SdpConnection::SdpConnection ()
    :
     networktype(NetworkTypeInternet),
     addresstype(AddressTypeIPV4),
     address("0.0.0.0"),
     multicast(0)
{
}


SdpConnection::SdpConnection (Data str)
    : multicast(0)

{
    deque<Data> connectionList;

    bool finished = false;

    while (!finished)
    {
	Data x = str.parse("/", &finished);
	if(finished)
	{
	    x = str;
	}
	connectionList.push_back(x);
    }

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
            cpDebug(LOG_DEBUG_STACK, "Multi-cast address in %s", str.logData() );
            //set the multicast specific parts.
            //create the multicast details.
            multicast = new SdpMulticast;
            assert(multicast);
	    LocalScopeAllocator lTtl;
            multicast->setTtl(atoi(connectionList[1].getData(lTtl)) );

        }
        if (connectionList.size() > 2)
        {
            assert(multicast);
	    LocalScopeAllocator lmulti;

            multicast->setnumAddr(atoi(connectionList[2].getData(lmulti) ));
        }
        //sub parms: get the address specific.
        //split again.
        //const char* str = connectionList[0].getData();

        Data str2 = connectionList[0];
        Data addressSpec(str2);


        // take out sub_split.  This may be causing corruption in the sdp.
        // -ctam

        deque<Data> addrSpec;
        finished = false;

	while (!finished)
	{
	    Data x = addressSpec.parse(" ", &finished);
	    if(finished)
	    {
		x = addressSpec;
	    }
	    addrSpec.push_back(x);
	}

	if(addrSpec.size() >= 3)
	{
	    // Use nTypeStr to make a copy of the addrSpec.
	    Data nTypeStr = addrSpec[0];
	    
	    if (nTypeStr == SdpNetworkTypeIN)
	    {
		networktype = NetworkTypeInternet;
	    }
	    else
	    {
		cpLog(LOG_ERR, "addressSpec: {%s}", addressSpec.logData());
		
		cpLog(LOG_ERR, 
                      "nTypeStr.logData()= <%s>, SdpNetworkTypeIN = <%s>",
		       nTypeStr.logData(), 
                      SdpNetworkTypeIN );
		
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
		multicast->setAddress(addrSpec[2]);
	    }
	    else
	    {
		address = addrSpec[2];
	    }
	}
    }
}


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
        address = connection.getUnicast();
    }
    return *(this);
}


SdpConnection::SdpConnection( const SdpConnection& connection )
    :networktype(connection.networktype),
     addresstype(connection.addresstype),
     address(connection.address)
{
    cpDebug(LOG_DEBUG_STACK, "SdpConnection copy constructor" );

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


Data
SdpConnection::networkTypeString () const
{
    Data s;

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


Data
SdpConnection::addressTypeString () const
{
    Data s;

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
}


Data
SdpConnection::encode () const
{
    Data s;

    s = "c=";
    s += networkTypeString();
    s += " ";
    s += addressTypeString();
    s += " ";
    if (multicast)
    {
        // xxx fix this when multicast gets a new-style encoder
//        multicast->encode (s);
    }
    else
    {
        s += address;
    }
    s += "\r\n";

    return s;
}


void
SdpConnection::setHold ()
{
    if ( !multicast )
    {
        address = "0.0.0.0";
    }
    else
    {
        multicast->setAddress("0.0.0.0");
    }
}


bool
SdpConnection::isHold ()
{
    Data addr;

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
{
}


void
SdpMulticast::encode (ostrstream& s)
{
    s << address << '/' << ttl;
    if (numAddress > 0)
    {
        s << '/' << numAddress;
    }
}
