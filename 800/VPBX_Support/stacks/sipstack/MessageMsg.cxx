/*
* $Log: MessageMsg.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: MessageMsg.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"
#include <cstdlib>
#include <cstdio>
#include "Data.hxx"
#include "cpLog.h"

#include "symbols.hxx"
#include "SystemInfo.hxx"

#include "MessageMsg.hxx"
#include "SipVia.hxx"
#include "SipContact.hxx"
#include "SipProxyAuthenticate.hxx"
#include "SipRequire.hxx"
#include "SipRetryAfter.hxx"
#include "SipRoute.hxx"
#include "SipServer.hxx"
#include "SipUnsupported.hxx"
#include "SipWarning.hxx"
#include "SipWwwAuthenticate.hxx"
#include "SipSubject.hxx"

using namespace Assist;


const char MessageMsg::INFO_SUBJECT[] = "Vovida MESSAGE";

MessageMsg::MessageMsg(): SipCommand()
{}


MessageMsg::~MessageMsg()
{
}

MessageMsg::MessageMsg(const MessageMsg& src)
        : SipCommand(src)
{
}


MessageMsg& MessageMsg::operator =(const MessageMsg& src)
{
    //copy details contained in the base class.
    if ( &src != this )
    {
        SipCommand::operator=(src);
    }
    return (*this);
}
    
bool MessageMsg::operator ==(const MessageMsg& src)
{
    return (*static_cast<SipCommand*>(this) == src );
}

MessageMsg::MessageMsg(Sptr <BaseUrl> toUrl, int fromPort)
        : SipCommand()
{
    cpDebug(LOG_DEBUG_STACK, "Constructor of MESSAGE");

    setInfoDetails(toUrl, fromPort);
}


MessageMsg::MessageMsg(const MessageMsg& msg, enum MessageMsgForward)
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

void MessageMsg::setInfoDetails(Sptr <BaseUrl> toUrl, int fromPort)
{
    //build and store in sipMessage.
    //build the non-SDP part here, and call SdpMsg class to build the SDP part.

    //build each header , and store it in that header class.
    cpDebug(LOG_DEBUG_STACK, "In MessageMsg");

    //create the RequestLine.
    SipRequestLine siprequestline(SIP_MESSAGE, toUrl);
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
    sipfrom.setHost( hip);
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

    SipCSeq cseq(SIP_MESSAGE, Data("1"));
    setCSeq(cseq);

    //create the subject.
    SipSubject subject(INFO_SUBJECT);
    setSubject(subject);

}


MessageMsg::MessageMsg( const Data& data )
        : SipCommand()
{
    SipCommand::decode(data);
}    // MessageMsg::MessageMsg( Data& data )


Method MessageMsg::getType() const
{
    return SIP_MESSAGE;
}

