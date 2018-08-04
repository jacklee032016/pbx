/*
* $Log: OptionsMsg.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: OptionsMsg.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/


#include "Data.hxx"
#include "OptionsMsg.hxx"
#include "SipContact.hxx"
#include "SipProxyAuthenticate.hxx"
#include "SipRequire.hxx"
#include "SipRetryAfter.hxx"
#include "SipRoute.hxx"
#include "SipSdp.hxx"
#include "SipServer.hxx"
#include "SipVia.hxx"
#include "SystemInfo.hxx"
#include "cpLog.h"

using namespace Assist;
using Assist::SDP::SdpSession;
using Assist::SDP::SdpConnection;
using Assist::SDP::SdpMedia;


OptionsMsg::OptionsMsg()
        : SipCommand()
{
    myRequestLine.setMethod(OPTIONS_METHOD);
    SipCSeq cseq( SIP_OPTIONS, 0 );
    setCSeq( cseq );
}


OptionsMsg::OptionsMsg(const OptionsMsg& src)
        : SipCommand(src)
{
}


OptionsMsg&
OptionsMsg::operator=( const OptionsMsg& src )
{
    if ( &src != this )
    {
        SipCommand::operator=( src );

        // Copy data member here
    }

    return ( *this );
}


bool 
OptionsMsg::operator ==(const OptionsMsg& src) 
{
   
   return (  *static_cast < SipCommand* > (this) == src);
   
}


OptionsMsg::OptionsMsg(Sptr <BaseUrl> toUrl, int listenPort, int rtpPort)
        : SipCommand()
{
    cpDebug(LOG_DEBUG_STACK, "C'tor of OptionsMsg");

    setOptionsDetails(toUrl, listenPort, rtpPort);
}


OptionsMsg::OptionsMsg(const OptionsMsg& msg, enum OptionsMsgForward)
{
    *this = msg;

    Sptr <BaseUrl> dest = getTo().getUrl();

    if ( getNumRoute() )
    {
        int numRoute = getNumRoute();
        if (numRoute != 0 )
        {
            dest = getRoute(0).getUrl();
            //remove that route.
            removeRoute(0);
        }
    }

    SipRequestLine reqLine = getRequestLine();
    reqLine.setUrl( dest );
    setRequestLine( reqLine );
}


void 
OptionsMsg::setOptionsDetails(Sptr <BaseUrl> toUrl, int listenPort, int rtpPort)
{
    //build and store in sipMessage.
    //build the non-SDP part here, and call SdpMsg class to build the SDP part.

    //build each header , and store it in that header class.
    cpDebug(LOG_DEBUG_STACK, "In OptionsMsg");

    //create the RequestLine.
    SipRequestLine siprequestline(SIP_OPTIONS, toUrl);
    setRequestLine(siprequestline);

    cpDebug(LOG_DEBUG_STACK, "Completed the requestLine");

	Data tmph=siprequestline.getHost();
	Data hip=theSystem.gethostAddress(&tmph);

    //create the Via header.
    SipVia sipvia;
    //copy the details of the current host, and port into the Via.

    sipvia.setHost(hip);
    //The default port number is set automatically.


    Data newport(listenPort);
    if (newport.length() > 0)
    {
        sipvia.setPort(newport.convertInt());
    }

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

    SipCSeq cseq(SIP_OPTIONS, Data("1"));
    SipMsg::setCSeq(cseq);

    //create the sdp.
    SipSdp sdp;
    //Change this so that we call functions in SDP, and SDP abstracts this
    //from us.
    // set connection details.
    SdpSession sdpDesc;

    Data connAddr = hip;
    SdpConnection connection;
    LocalScopeAllocator lo;
    connection.setUnicast(connAddr.getData(lo));
    sdpDesc.setConnection(connection);

    SdpMedia* media = new SdpMedia;
    media->setPort(rtpPort);

    sdpDesc.addMedia(media);
    //If possible use NTP time.
    srandom((unsigned int)time(NULL));
    int verStr = random();


    //If possible , use NTP time.
    srandom((unsigned int)time(NULL));
    int sessionStr = random();

    sdpDesc.setVersion(verStr);
    sdpDesc.setSessionId(sessionStr);

    sdp.setSdpDescriptor(sdpDesc);
    sdp.setUserName(theSystem.getUserName());

    sdp.setSessionName(theSystem.getSessionName());
    sdp.setAddress(hip);
    setContentData(&sdp, 0);
}


OptionsMsg::OptionsMsg( const Data& data )
        : SipCommand()
{

    SipCommand::decode(data);

}


Method 
OptionsMsg::getType() const
{
    return SIP_OPTIONS;
}

