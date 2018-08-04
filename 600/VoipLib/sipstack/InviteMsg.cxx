/*
 * $Id: InviteMsg.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */

#include "Data.hxx"
#include "InviteMsg.hxx"
#include "SipAuthorization.hxx"
#include "SipContact.hxx"
#include "SipOspAuthorization.hxx"
#include "SipProxyAuthenticate.hxx"
#include "SipRequire.hxx"
#include "SipRetryAfter.hxx"
#include "SipRoute.hxx"
#include "SipSdp.hxx"
#include "SipServer.hxx"
#include "SipSubject.hxx"
#include "SipUnsupported.hxx"
#include "SipVia.hxx"
#include "SipWarning.hxx"
#include "SipWwwAuthenticate.hxx"
#include "SystemInfo.hxx"
#include "cpLog.h"
#include "SipTo.hxx"

using namespace Vocal;
using Vocal::SDP::SdpMedia;
using Vocal::SDP::SdpSession;
using Vocal::SDP::SdpConnection;


InviteMsg::InviteMsg()
        : SipCommand()
{
    myRequestLine.setMethod(INVITE_METHOD);
    SipCSeq cseq( SIP_INVITE, Data(0) );
    setCSeq( cseq );
}



InviteMsg::InviteMsg(const InviteMsg& src, Sptr <BaseUrl> url)
{
    *this = src;

    myRequestLine.setUrl(url);
    //create the callId header
    
    setCallId(SipCallId());
}

InviteMsg::InviteMsg(const InviteMsg& src)
        : SipCommand(src)
{
    myRequestLine.setMethod(INVITE_METHOD);
}


InviteMsg::InviteMsg(const InviteMsg& msg, enum InviteMsgForward)
{
    *this = msg;

    Sptr <BaseUrl> dest = getTo().getUrl();

    int numRoute = getNumRoute();

    if ( numRoute )
    {
        dest = getRoute(0).getUrl();
        //remove that route.
        removeRoute(0);
    }
    else
    {
        dest = getTo().getUrl();
    }
    myRequestLine.setUrl(dest);
}


///
InviteMsg&
InviteMsg::operator=( const InviteMsg& src )
{
    if (&src != this )
    {
        SipCommand::operator=( src );

    }

    return ( *this );
}


bool 
InviteMsg::operator ==(const InviteMsg& src) const
{
    return SipCommand::operator==(src);
}


InviteMsg::InviteMsg(const StatusMsg& statusMsg)
    : SipCommand(statusMsg)
{
}


/// UAS uses
InviteMsg::InviteMsg(const SipCommand& src,
                     const SipVia& via,
                     const SipCSeq& cseq)
    : SipCommand(src, via, cseq)
{
}


InviteMsg::InviteMsg(Sptr <BaseUrl> toUrl, int listenPort, int rtpPort)
        : SipCommand()
{
    assert(myMimeList.getNumContentData() == 0);
    cpDebug(LOG_DEBUG_STACK, "C'tor of Invite");

    if (toUrl.getPtr() != 0)
    {
	cpDebug(LOG_DEBUG_STACK, "SipUrl: %s", toUrl->encode().logData());
	setInviteDetails(toUrl, listenPort, rtpPort);
    }
}


void 
InviteMsg::setInviteDetails(Sptr <BaseUrl> toUrl, int listenPort, int rtpPort)
{
    if (toUrl.getPtr() != 0)
    {
	cpDebug(LOG_DEBUG_STACK, "SipUrl:setInviteDetails: %s", toUrl->encode().logData());
    }
    //build and store in sipMessage.
    //build the non-SDP part here, and call SdpMsg class to build the SDP part.

    //build each header , and store it in that header class.
    cpDebug(LOG_DEBUG_STACK, "In InviteMsg");

    //create the RequestLine.
    SipRequestLine siprequestline(SIP_INVITE, toUrl);
    setRequestLine(siprequestline);

    cpDebug(LOG_DEBUG_STACK, "Completed the requestLine");

	Data tmph,hip;
	if(toUrl.getPtr()!=0)
	{
		 tmph=siprequestline.getHost();
		 hip=theSystem.gethostAddress(&tmph);
	}
	else
		hip=theSystem.gethostAddress();

	
    //create the Via header.
    SipVia sipvia;

    sipvia.setHost(hip);
    //The default port number is set automatically.

    Data newport(listenPort);
    if (newport.length() > 0)
    {
        sipvia.setPort(newport.convertInt());
    }

    setVia(sipvia, 0);

    cpDebug(LOG_DEBUG_STACK, "Completed the via header");

    //create the From header
    SipFrom sipfrom;
    //getUserName, and getDisplayName are extern functions,
    //which contains the user's details. Set from command line?
    Data user = theSystem.getUserName();
    sipfrom.setUser(user);
    Data dispName = theSystem.getDisplayName();
    sipfrom.setDisplayName(dispName);
    Data port(listenPort);

    sipfrom.setHost( hip );
    sipfrom.setPort(port);
	
    setFrom(sipfrom);
   
    cpDebug(LOG_DEBUG_STACK, "Completed the from header");

    //create the To header
    Sptr <BaseUrl> requestUrl = siprequestline.getUrl();

    SipTo sipto(requestUrl);
	
    if (toUrl.getPtr() != 0)
    {
	Data name = toUrl->getUserValue();
	sipto.setDisplayName(name);
    }
  
    setTo(sipto);

    cpDebug(LOG_DEBUG_STACK, "Completed the to header");

    //create the callId header
    SipCallId callId;
    setCallId(callId);

    cpDebug(LOG_DEBUG_STACK, "Completed the call id header");

    //create the CSeq header

    Data cseqval = "1";
    SipCSeq cseq(SIP_INVITE, cseqval);
    SipMsg::setCSeq(cseq);

    //create the subject.
    SipSubject subject("Assist_SIP_INVITE");
    setSubject(subject);

    //set Contact.  This is MUST in Invite.
    SipContact contact;
    contact.setUrl(sipfrom.getUrl());
    contact.setDisplayName(sipfrom.getDisplayName());
    setContact(contact);

    SipContentType type(Data("application"), Data("SDP") );
    setContentType(type);
    //create the sdp.
    SipSdp sdp;
    //Change this so that we call functions in SDP, and SDP abstracts this
    //from us.
    // set connection details.
    SdpSession sdpDesc;

    Data hostAddr = hip;
    SDP::AddressType protocol = SDP::AddressTypeIPV4;

    SdpConnection connection;
    LocalScopeAllocator lo;
    connection.setUnicast(hostAddr.getData(lo));
    
    // There is probably a better way to do this...
    if(hostAddr.find(":") != Data::npos)
	    protocol = SDP::AddressTypeIPV6;

    connection.setAddressType(protocol);

    sdpDesc.setConnection(connection);

    SdpMedia* media = new SdpMedia;
    media->setPort(rtpPort);

    sdpDesc.addMedia(media);
    srandom((unsigned int)time(NULL));
    int verStr = random();

    srandom((unsigned int)time(NULL));
    int sessionStr = random();

    sdpDesc.setVersion(verStr);
    sdpDesc.setSessionId(sessionStr);

    //To further perform any other operation on that particular media
    //item, use loc.
    sdp.setSdpDescriptor(sdpDesc);
    sdp.setUserName(theSystem.getUserName());

    sdp.setSessionName(theSystem.getSessionName());
    sdp.setAddress(hostAddr);
    sdp.setAddressType(protocol);
    setContentData(&sdp, 0);
}


InviteMsg::InviteMsg( const Data& data ) throw (SipParserException&)
        : SipCommand()
{
    try
    {
        SipCommand::decode(data);
    }
    catch (SipParserException& e)
    {
        cpLog(LOG_ERR, "Failed to parse the SIP msg, since: %s", 
              e.getDescription().c_str());
        throw SipParserException(e.getDescription(),
                                 __FILE__,
                                 __LINE__);
    }
}    // InviteMsg::InviteMsg( Data& data )


Method InviteMsg::getType() const
{
    return SIP_INVITE;
}

const SipSessionExpires& InviteMsg::getSessionExpires() const
{
    Sptr<SipSessionExpires> x;
    myHeaderList.getParsedHeader(x, SIP_SESSION_EXPIRES_HDR);
    return *x;
}

const SipMinSE& InviteMsg::getMinSE() const 
{
    Sptr<SipMinSE> x;
    myHeaderList.getParsedHeader(x, SIP_MIN_SE_HDR);
    return *x;
}

void InviteMsg::setSessionExpires(const SipSessionExpires& sess)
{
    Sptr<SipSessionExpires> x;
    myHeaderList.getParsedHeader(x, SIP_SESSION_EXPIRES_HDR);
    *x = sess;
}
void InviteMsg::setMinSE(const SipMinSE& minSe)
{
    Sptr<SipMinSE> x;
    myHeaderList.getParsedHeader(x, SIP_MIN_SE_HDR);
    *x = minSe;
}
