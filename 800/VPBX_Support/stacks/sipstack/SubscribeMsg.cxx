/*
* $Log: SubscribeMsg.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:10  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SubscribeMsg.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "SipCallLeg.hxx"
#include "SipExpires.hxx"
#include "SipMsg.hxx"
#include "SipRoute.hxx"
#include "SipVia.hxx"
#include "SubscribeMsg.hxx"
#include "SystemInfo.hxx"
#include "cpLog.h"
#include "symbols.hxx"

using namespace Assist;


SubscribeMsg::SubscribeMsg()
        : SipCommand()
{
    myRequestLine.setMethod(SUBSCRIBE_METHOD);
    SipCSeq cseq( SIP_SUBSCRIBE, Data(0) );
    setCSeq( cseq );

    //set to and from of host creating SUBSCRIBE msg
    SipFrom from;
    Data user = theSystem.getUserName();
    from.setUser(user);
    Data dispName = theSystem.getDisplayName();
    from.setDisplayName(dispName);
    from.setHost( Data(theSystem.gethostAddress()) );
    from.setPort(Data(5060));  //port defaults to 5060
    setFrom(from);

    SipTo to( from.getUrl());
    to.setDisplayName( from.getDisplayName());
    setTo(to);

    //use the register url from the system as the request url, if it exists.  If it
    //doesn't exist, use the to url as the request url.
    Sptr <BaseUrl> requestUrl;
    Data registerDomain = theSystem.getRegisterDomain();
    if (registerDomain.length() == 0)
    {
        //get from the To field.
        registerDomain = to.getHost();
    }
    if (requestUrl.getPtr() != 0)
    {
	if (requestUrl->getType() == SIP_URL)
	{
	    Sptr <SipUrl> sipUrl;
	    
	    (sipUrl.dynamicCast(requestUrl))->setHost(registerDomain);
	}
    }
    setSubscribeDetails(from.getUrl(), requestUrl);
}

    
SubscribeMsg::SubscribeMsg(const Data& data)
        : SipCommand()
{
    try
    {
        SipCommand::decode(data);
    }
    catch (SipParserException& e)
    {
        cpLog(LOG_ERR, "Failed to parse the SIP msg, since: %s", e.getDescription().c_str());
    }
}


//call-member subscriptions
SubscribeMsg::SubscribeMsg(const SipCallLeg& callLeg, Sptr <BaseUrl> reqUrl)
        : SipCommand()
{
    //set call-id, from, and to
    setCallId(callLeg.getCallId());
    setTo(callLeg.getTo());
    setFrom(callLeg.getFrom());

    //set the other headers of SUBSCRIBE message
    Sptr <BaseUrl> fromUrl = callLeg.getFrom().getUrl();
    setSubscribeDetails(fromUrl, duplicateUrl(reqUrl));
}


//third-party subscriptions
SubscribeMsg::SubscribeMsg(const SipCallId& callId, const SipTo& to, int port)
        : SipCommand()
{
    //set the call-id and to
    setCallId(callId);
    setTo(to);

    //set from
    SipFrom from;
    Data user = theSystem.getUserName();
    from.setUser(user);
    Data dispName = theSystem.getDisplayName();
    from.setDisplayName(dispName);
    Data listenPort(port);
	Data tmph=to.getHost();
	Data hip=theSystem.gethostAddress(&tmph);
    from.setHost( hip );
    from.setPort(listenPort);
    setFrom(from);

    //set the other headers of SUBSCRIBE message
    Sptr <BaseUrl> toUrl = to.getUrl();
    Sptr <BaseUrl> fromUrl = from.getUrl();
    setSubscribeDetails(fromUrl, toUrl);
}

    
//copy constructor
SubscribeMsg::SubscribeMsg(const SubscribeMsg& src)
        : SipCommand(src)
{}


SubscribeMsg::SubscribeMsg( const SubscribeMsg& msg, enum SubscribeMsgForward )
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


SubscribeMsg::~SubscribeMsg()
{}


SubscribeMsg& SubscribeMsg::operator=(const SubscribeMsg& src)
{
    if (&src != this)
    {
        SipCommand::operator=(src);
    }
    return (*this);
}

    
bool SubscribeMsg::operator ==(const SubscribeMsg& src) 
{
   
   return (  *static_cast < SipCommand* > (this) == src);
   
}


Method 
SubscribeMsg::getType() const
{
    return SIP_SUBSCRIBE;
}

void 
SubscribeMsg::setSubscribeDetails(Sptr <BaseUrl> fromUrl, 
                                       Sptr <BaseUrl> toUrl)
{
    //set the CSeq
    SipCSeq cseq(SIP_SUBSCRIBE, Data(1));
    setCSeq(cseq);

    //set the Via
    SipVia sipVia;
    sipVia.setprotoVersion("2.0");
    if (fromUrl.getPtr() != 0)
    {
	if (fromUrl->getType() == SIP_URL)
	{
	    Sptr <SipUrl> sipUrl;
	    sipVia.setHost((sipUrl.dynamicCast(fromUrl))->getHost());
	    sipVia.setPort((sipUrl.dynamicCast(fromUrl))->getPort().convertInt());
	}
    }
    setVia(sipVia);

    //add default Expires
    SipExpires sipexpires;
    sipexpires.setDelta(Data("3600"));
    setExpires(sipexpires);

    //set the request line
    SipRequestLine requestLine(SIP_SUBSCRIBE, toUrl);
    setRequestLine(requestLine);
}
