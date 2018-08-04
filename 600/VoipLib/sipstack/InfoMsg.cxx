/*
 * $Id: InfoMsg.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */

#include "cpLog.h"
#include "Data.hxx"
#include "symbols.hxx"
#include "InfoMsg.hxx"
#include "SipContact.hxx"
#include "SipProxyAuthenticate.hxx"
#include "SipRequire.hxx"
#include "SipRetryAfter.hxx"
#include "SipRoute.hxx"
#include "SipServer.hxx"
#include "SipSubject.hxx"
#include "SipUnsupported.hxx"
#include "SipVia.hxx"
#include "SipWarning.hxx"
#include "SipWwwAuthenticate.hxx"
#include "SystemInfo.hxx"

using namespace Vocal;


const char InfoMsg::INFO_SUBJECT[] = "VovidaINFO";

InfoMsg::InfoMsg(): SipCommand()
{}


InfoMsg::~InfoMsg()
{
}

InfoMsg::InfoMsg(const InfoMsg& src)
        : SipCommand(src)
{}


InfoMsg& InfoMsg::operator =(const InfoMsg& src)
{
    //copy details contained in the base class.
    if ( &src != this )
    {
        SipCommand::operator=(src);
    }
    return (*this);
}
    
bool InfoMsg::operator ==(const InfoMsg& src)
{
    return (*static_cast<SipCommand*>(this) == src );
}

InfoMsg::InfoMsg(Sptr <BaseUrl> toUrl, int fromPort)
        : SipCommand()
{
    cpDebug(LOG_DEBUG_STACK, "C'tor of Info");

    setInfoDetails(toUrl, fromPort);
}


InfoMsg::InfoMsg(const InfoMsg& msg, enum InfoMsgForward)
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

    SipRequestLine reqLine = getRequestLine();
    reqLine.setUrl( dest );
    setRequestLine( reqLine );
}

void InfoMsg::setInfoDetails(Sptr <BaseUrl> toUrl, int fromPort)
{
    //build and store in sipMessage.
    //build the non-SDP part here, and call SdpMsg class to build the SDP part.

    //build each header , and store it in that header class.
    cpDebug(LOG_DEBUG_STACK, "In InfoMsg");

    //create the RequestLine.
    SipRequestLine siprequestline(SIP_INFO, toUrl);
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


    sipvia.setPort(fromPort);

    Data lversion("2.0");
    sipvia.setprotoVersion(lversion);  // do we need to get this from some place.
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
    Data port(fromPort);
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

    SipCSeq cseq(SIP_INFO, Data("1"));
    setCSeq(cseq);

    //create the subject.
    SipSubject subject(INFO_SUBJECT);
    setSubject(subject);

}


InfoMsg::InfoMsg( const Data& data )
        : SipCommand()
{
    SipCommand::decode(data);
}    // InfoMsg::InfoMsg( Data& data )


Method InfoMsg::getType() const
{
    return SIP_INFO;
}
