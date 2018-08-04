/*
* $Log: SipSdp.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:10  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipSdp.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/
#include "global.h"
#include "SipSdp.hxx"
#include <cassert>
#include "cpLog.h"

using namespace Assist;

using Assist::SDP::SdpSession;
using Assist::SDP::SdpConnection;
using Assist::SDP::SdpMedia;


SipSdp::SipSdp()
    : sdpDescriptor()
{
    setContentType(new SipContentType("application", "sdp"));
}


SipSdp::SipSdp(const SdpSession& descriptor)
    :SipContentData(),
     sdpDescriptor(descriptor)
{
    setContentType(new SipContentType("application", "sdp"));
}

SipSdp::~SipSdp()
{
}


SipSdp&
SipSdp::operator =(const SipSdp& src)
{
   
    if (&src != this )
    {
	SipContentData::operator=(src);
        sdpDescriptor = src.sdpDescriptor;
    }
    return (*this);
}


#if 0
bool
SipSdp::operator ==(const SipSdp& rhs) const
{
    // xxx not implemented
    assert(0);
    if ( &rhs == this )
    {
        return true;
    }
    else if (rhs.sdpDescriptor == sdpDescriptor)
    {
        return true;
    }
    else
    {
        return false;
    }
}
#endif


SipSdp::SipSdp(const SipSdp& src)
        : SipContentData(src),
	  sdpDescriptor(src.sdpDescriptor)
{
    setContentType(new SipContentType("application", "sdp"));
}


SipSdp::SipSdp( const Data& data )
    : SipContentData()
{
    sdpDescriptor.decode(data);
    setContentType(new SipContentType("application", "sdp"));
}

SdpSession&
SipSdp::getSdpDescriptor() 
{
    return sdpDescriptor;
}

    
void
SipSdp::setSdpDescriptor(const SdpSession& sdp)
{
    sdpDescriptor = sdp;
}

bool
SipSdp::doCodecsMatch() const // logic for this will be in sdp stack
{
    // What's up with TODO?  -ctam

    //TODO
    return true;
}

    
void
SipSdp::setConnAddress(const SdpConnection& connAddr)
{
    sdpDescriptor.setConnection(connAddr);
}


Data
SipSdp::getConnAddress()
{
    Data data;
    SdpConnection* conn = sdpDescriptor.getConnection();
    if (conn)
    {
        data = conn->getUnicast();
    }

    return data;
}


void
SipSdp::setUserName(const Data& name)
{
    LocalScopeAllocator lo;
    sdpDescriptor.setUserName(name.getData(lo));
}


Data
SipSdp::getUserName()
{
    return Data(sdpDescriptor.getUserName());
}

void
SipSdp::setAddressType(SDP::AddressType addrType)
{
    sdpDescriptor.setAddressType(addrType);   
}

SDP::AddressType
SipSdp::getAddressType()
{
    return sdpDescriptor.getAddressType();   
}

void
SipSdp::setAddress(const Data& address)
{
    LocalScopeAllocator lo;
    sdpDescriptor.setAddress(address.getData(lo));   
}


Data
SipSdp::getAddress()
{
    return Data(sdpDescriptor.getAddress());
}



void
SipSdp::setRtpPort(int port)
{
    list < SdpMedia* > mediaList = sdpDescriptor.getMediaList();
    if (mediaList.size() > 0)
    {
        (*mediaList.begin())->setPort(port);
    }
    else
    {
        SdpMedia* media = new SdpMedia;
        media->setPort(port);
        sdpDescriptor.addMedia (media);
    }
}


int
SipSdp::getRtpPort() const
{
    list < SdpMedia* > mediaList = sdpDescriptor.getMediaList();
    if (mediaList.size() > 0)
    {
        return (*mediaList.begin())->getPort();
    }
    else
    {
        //TODO Throw an exception
        return -1;
    }
}


void
SipSdp::setSessionName(const Data& session)
{ 
    LocalScopeAllocator lo;
    sdpDescriptor.setSessionName(session.getData(lo));
}


Data
SipSdp::getSessionName()
{
    return Data(sdpDescriptor.getSessionName());
}

void
SipSdp::setSessionId(unsigned int sessionId)
{
    sdpDescriptor.setSessionId(sessionId);
}
 
 
unsigned int
SipSdp::getSessionId()
{
    return (sdpDescriptor.getSessionId());
}


Data
SipSdp::encodeBody(int& len)
{
    Data encsdp = sdpDescriptor.encode();
    
    len = encsdp.length();

    if (getContentLength() != 0)
    {
	getContentLength()->setLength(len);
    }
    return encsdp;
}


SipContentData* 
SipSdp::duplicate() const
{
    return new SipSdp(*this);
}

std::ostream&
Assist::operator<<(std::ostream& s, SipSdp& msg)
{
    int l=0;
    s << msg.encodeBody(l);
    return s;
}
