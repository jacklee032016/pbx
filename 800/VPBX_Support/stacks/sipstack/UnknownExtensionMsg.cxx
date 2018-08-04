/*
* $Log: UnknownExtensionMsg.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:10  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: UnknownExtensionMsg.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include <cstdlib>
#include <cstdio>
#include "Data.hxx"
#include "cpLog.h"
#include "symbols.hxx"
#include "SystemInfo.hxx"

#include "UnknownExtensionMsg.hxx"

#include "SipVia.hxx"
#include "SipContact.hxx"
#include "SipProxyAuthenticate.hxx"
#include "SipRequire.hxx"
#include "SipRetryAfter.hxx"
#include "SipRoute.hxx"
#include "SipServer.hxx"
#include "SipSdp.hxx"

using Assist::SDP::SdpMedia;
using Assist::SDP::SdpConnection;
using Assist::SDP::SdpSession;

using namespace Assist;

UnknownExtensionMsg::UnknownExtensionMsg()
        : SipCommand()
{
}


UnknownExtensionMsg::UnknownExtensionMsg(const UnknownExtensionMsg& src)
        : SipCommand(src)
{
}


UnknownExtensionMsg&
UnknownExtensionMsg::operator=( const UnknownExtensionMsg& src )
{
    if ( &src != this )
    {
        SipCommand::operator=( src );

        // Copy data member here
    }

    return ( *this );
}


bool 
UnknownExtensionMsg::operator ==(const UnknownExtensionMsg& src) 
{
   return (  *static_cast < SipCommand* > (this) == src);
}


UnknownExtensionMsg::UnknownExtensionMsg(Sptr<BaseUrl> toUrl, int listenPort, int rtpPort)
        : SipCommand()
{
    cpDebug(LOG_DEBUG_STACK, "C'tor of UnknownExtensionMsg");

    setUnknownExtensionMsgDetails(toUrl, listenPort, rtpPort);
}


UnknownExtensionMsg::UnknownExtensionMsg(const UnknownExtensionMsg& msg, enum UnknownExtensionMsgForward)
{
    *this = msg;

    Data test1 = getRequestLine().getMethod();

    Data test2 = getCSeq().getMethod();
    if (test1 == test2)
    {
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
	if (dest.getPtr() != 0)
	{
	    SipRequestLine reqLine = getRequestLine();
	    reqLine.setUrl( dest );
	    setRequestLine( reqLine );
	}
    }
    else
    {
        //EXCEPTION
    }

}


void UnknownExtensionMsg::setUnknownExtensionMsgDetails(Sptr <BaseUrl> toUrl, int listenPort, int rtpPort)
{
    //build and store in sipMessage.
    //build the non-SDP part here, and call SdpMsg class to build the SDP part.

    //build each header , and store it in that header class.
    cpDebug(LOG_DEBUG_STACK, "In UnknownExtensionMsg");

    //create the RequestLine.
    SipRequestLine siprequestline;
    Data method1 = siprequestline.getMethod();
    SipRequestLine siprequestline1(method1, toUrl);

	Data tmph,hip;
	if(toUrl.getPtr()!=0)
	{
		 tmph=siprequestline1.getHost();
		 hip=theSystem.gethostAddress(&tmph);
	}
	else
		hip=theSystem.gethostAddress();
	
    setRequestLine(siprequestline);
    SipCSeq cseq1;
    Data cseqval = "1";
    Data method2 = cseq1.getMethod();
   
    setCSeq(SipCSeq(method2, cseqval));

    cpDebug(LOG_DEBUG_STACK, "Completed the requestLine");

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
   
    sipvia.setprotoVersion(Data("2.0"));  // do we need to get this from some place.
    setVia(sipvia, 0);

    cpDebug(LOG_DEBUG_STACK, "Completed the via header");

    //create the From header
    SipFrom unknownMsgFrom;
    //getUserName, and getDisplayName are extern functions,
    //which contains the user's details. Set from command line?
    Data user = theSystem.getUserName();
    unknownMsgFrom.setUser(user);
    Data dispName = theSystem.getDisplayName();
    unknownMsgFrom.setDisplayName(dispName);
    Data port(listenPort);
    unknownMsgFrom.setHost( hip );
    unknownMsgFrom.setPort(port);
    setFrom(unknownMsgFrom);

    cpDebug(LOG_DEBUG_STACK, "Completed the from header");


    //create the To header
    Sptr <BaseUrl> requestUrl = siprequestline.getUrl();

   
    if (toUrl.getPtr() != 0)
    {
	SipTo unknownMsgTo(requestUrl);
	Data name = toUrl->getUserValue();
	unknownMsgTo.setDisplayName(name);
	setTo(unknownMsgTo);
    }

    cpDebug(LOG_DEBUG_STACK, "Completed the to header");

    //create the callId header
    setCallId(SipCallId());

    cpDebug(LOG_DEBUG_STACK, "Completed the call id header");

    //create the sdp.
    SipSdp sdp;
    //Change this so that we call functions in SDP, and SDP abstracts this
    //from us.
    // set connection details.
    SdpSession sdpDesc;

    Data connAddr = hip;
    /*char connAddrstr[256];
    strcpy(connAddrstr, connAddr); */
    SdpConnection connection;
    LocalScopeAllocator lo;
    connection.setUnicast(connAddr.getData(lo));
    sdpDesc.setConnection(connection);

    SdpMedia* media = new SdpMedia;
    media->setPort(rtpPort);

    sdpDesc.addMedia(media);
    //Change this to use NTP time
    srandom((unsigned int)time(0));
    int verStr = random();


    //Change this to use  NTP time
    srandom((unsigned int)time(0));
    int sessionStr = random();

    sdpDesc.setVersion(verStr);
    sdpDesc.setSessionId(sessionStr);

    sdp.setSdpDescriptor(sdpDesc);
    sdp.setUserName(theSystem.getUserName());

    sdp.setSessionName(theSystem.getSessionName());
    sdp.setAddress(hip);
    setContentData(&sdp, 0);
}


UnknownExtensionMsg::UnknownExtensionMsg( Data& data )
        : SipCommand()
{
    SipCommand::decode(data);
}


Method 
UnknownExtensionMsg::getType() const
{
    return SIP_UNKNOWN;
}
