/*
 * $Id: RegisterMsg.cxx,v 1.4 2007/03/17 19:56:23 lizhijie Exp $
 */

#include "Data.hxx"
#include "RegisterMsg.hxx"
#include "SipCSeq.hxx"
#include "SipCallId.hxx"
#include "SipContact.hxx"

#include "SipProxyAuthenticate.hxx"
#include "SipRequestLine.hxx"
#include "SipRequire.hxx"
#include "SipRoute.hxx"
#include "SipServer.hxx"
#include "SipTo.hxx"
#include "SipFrom.hxx"
#include "SipUnsupported.hxx"
#include "SipUrl.hxx"
#include "SipVia.hxx"
#include "SipWarning.hxx"
#include "SipWwwAuthenticate.hxx"
#include "SystemInfo.hxx"
#include "cpLog.h"

using namespace Vocal;


RegisterMsg::RegisterMsg()
        : SipCommand()
{
    setRegisterDetails();
}


RegisterMsg::RegisterMsg(const RegisterMsg& src)
        : SipCommand(src)
{
}


RegisterMsg::RegisterMsg(const Data& data)
        : SipCommand()
{
    SipCommand::decode(data);
}


RegisterMsg::RegisterMsg(const RegisterMsg& msg, enum RegisterMsgForward)
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


RegisterMsg&
RegisterMsg::operator=( const RegisterMsg& src )
{
    if ( &src != this )
    {
        SipCommand::operator=( src );

        // Copy data member here
    }

    return ( *this );
}


bool 
RegisterMsg::operator ==(const RegisterMsg& src) 
{
   
   return (  *static_cast < SipCommand* > (this) == src);
   
}


void RegisterMsg::setRegisterDetails()
{
	cpDebug(LOG_DEBUG_STACK, "In setRegisterDetails");
	cpDebug(LOG_DEBUG_STACK, "setRegisterDetails\n");
    //construct the To header.
    Data toString = theSystem.getUrlToRegister(); //should return sip:....
    Sptr <BaseUrl> toUrl;

    if (toString.length() == 0)
    {
        toString = "sip:";
        toString += theSystem.gethostAddress();
	toUrl = BaseUrl::decode(toString);
    }
    else
    {
        toUrl = BaseUrl::decode(toString);
    }

    SipTo sipto(toUrl);
    setTo(sipto);

    //construct the From header.
    SipFrom sipfrom;
    Data user = theSystem.getUserName();
    sipfrom.setUser(user);
    Data dispName = theSystem.getDisplayName();

    sipfrom.setHost(theSystem.gethostAddress());
    setFrom(sipfrom);

    //Construct the request line.
    //This is the domain of the registrar. Read from theSystem, or get from the To
    //field.
    Sptr <BaseUrl> requestUrl;
    Data registerDomain = theSystem.getRegisterDomain();
    if (registerDomain.length() == 0)
    {
        //get from the To field.
        registerDomain = sipto.getHost();
    }
    if (requestUrl.getPtr() != 0)
    {
	if (requestUrl->getType() == SIP_URL)
	{
	    Sptr <SipUrl> sipUrl;
	    sipUrl.dynamicCast(requestUrl);
    
	    sipUrl->setHost(registerDomain);
	}
    }
    SipRequestLine requestLine(SIP_REGISTER, requestUrl);

    setRequestLine(requestLine);

    //set the Via.
    SipVia via;

	Data hip=theSystem.gethostAddress();
    via.setHost(hip);


    setVia(via);

    SipCallId callId;
    setCallId(callId);

    //create the CSeq header

    Data cseqval = "1";
    SipCSeq cseq(SIP_REGISTER, cseqval);
    SipMsg::setCSeq(cseq);

    //add default Expires
    SipExpires sipexpires;
    sipexpires.setDelta(Data("3600"));
    SipMsg::setExpires(sipexpires);

    //SipContact contact() TODO after SipContact is fully implemented.
}


Method RegisterMsg::getType() const
{
    return SIP_REGISTER;
}

