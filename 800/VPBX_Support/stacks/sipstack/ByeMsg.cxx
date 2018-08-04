/*
* $Log: ByeMsg.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: ByeMsg.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"
#include "ByeMsg.hxx"
#include "SipAuthorization.hxx"
#include "SipContact.hxx"
#include "SipContentDisposition.hxx"
#include "SipEncryption.hxx"
#include "SipFrom.hxx"
#include "SipHide.hxx"
#include "SipMaxForwards.hxx"
#include "SipProxyAuthorization.hxx"
#include "SipProxyRequire.hxx"
#include "SipRequire.hxx"
#include "SipRecordRoute.hxx"
#include "SipRoute.hxx"
#include "SipVia.hxx"
#include "StatusMsg.hxx"

using namespace Assist;


//ByeMsg::ByeMsg(const SipCommand& ackCommand)

/* If this is the caller(issued INVITE, then the Bye Msg should be
sent to the Contact obtained in the 200 Status msg.  If this is the
callee, then the Bye Msg should be sent to the Contact obtained in the
INVITE msg , or the ACK msg.  It is up to the User Agent to send the
corrent SipMsg from which the Contact will be taken */

ByeMsg::ByeMsg() : SipCommand()
{
    myRequestLine.setMethod(BYE_METHOD);
    SipCSeq cseq( SIP_BYE, 0 );
    setCSeq( cseq );
}


ByeMsg::ByeMsg(const ByeMsg& src) : SipCommand(src)
{
}

ByeMsg::ByeMsg(const SipCommand& src)
        : SipCommand(src)
{
    myRequestLine.setMethod(BYE_METHOD);
    SipCSeq cseq = src.getCSeq();
    int value = cseq.getCSeqData().convertInt();
    value++;
    SipCSeq newcseq(SIP_BYE, Data(value));
    setCSeq(newcseq);
   // Flip To & From when using ACK to generate BYE
    const SipFrom& srcfrom = src.getFrom();
    const SipTo& srcto = src.getTo();

    //set the To header.

    SipTo to(srcfrom.getUrl());
    to.setDisplayName( srcfrom.getDisplayName() );
    to.setTag( srcfrom.getTag() );
    to.setToken( srcfrom.getToken() );
    to.setQstring( srcfrom.getQstring() );
    setTo(to);

    //set the From header.
    SipFrom from(srcto.getUrl());
    from.setDisplayName( srcto.getDisplayName() );
    from.setTag( srcto.getTag() );
    from.setToken( srcto.getToken() );
    from.setQstring( srcto.getQstring() );
    setFrom(from);
}


ByeMsg::ByeMsg(const SipCommand& src, const SipVia& via, const SipCSeq& cseq)
        : SipCommand(src, via, cseq)
{
    myRequestLine.setMethod(BYE_METHOD);
}


ByeMsg::ByeMsg(const StatusMsg& statusMsg)
        : SipCommand(statusMsg)
{
    myRequestLine.setMethod(BYE_METHOD);

    SipCSeq cseq = statusMsg.getCSeq();
    int value = cseq.getCSeqData().convertInt();

    value++;

    SipCSeq newcseq(SIP_BYE, Data(value));
    setCSeq(newcseq);
}


ByeMsg::ByeMsg(const ByeMsg& msg, enum ByeMsgForward)
{
    *this = msg;

    Sptr<BaseUrl> dest = getTo().getUrl();

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


ByeMsg& ByeMsg::operator=(const ByeMsg& src)
{
    if ( &src != this)
    {
        *static_cast < SipCommand* > (this) = src;
    }
    return (*this);
}

bool ByeMsg::operator ==(const ByeMsg& src) 
{
   
   return (  *static_cast < SipCommand* > (this) == src);
   
}

ByeMsg::ByeMsg(const Data& data)
        : SipCommand()
{
        SipCommand::decode(data);  
}


void ByeMsg::setByeDetails( const SipMsg& sipMessage)
{
    // TODO:  what should listenPort mean???
    //We will use the default port:5060.

    /*** swap from and to only if callee(got INVITE or ACK) . If
     * caller, then don't swap */
    //check the Url for the Method.

    if ( dynamic_cast < const SipCommand* > (&sipMessage) )
    {
        const SipFrom& srcfrom = sipMessage.getFrom();
        const SipTo& srcto = sipMessage.getTo();
        //set the To header.
        SipTo to(srcfrom.getUrl());
        to.setDisplayName( srcfrom.getDisplayName() );
        to.setTag( srcfrom.getTag() );
        to.setToken( srcfrom.getToken() );
        to.setQstring( srcfrom.getQstring() );
        setTo(to);

        //set the From header.
        SipFrom from(srcto.getUrl());
        from.setDisplayName( srcto.getDisplayName() );
        from.setTag( srcto.getTag() );
        from.setToken( srcto.getToken() );
        from.setQstring( srcto.getQstring() );
        setFrom(from);

    }
    else if ( dynamic_cast < const StatusMsg* > (&sipMessage) )
    {
        setTo(sipMessage.getTo());
        setFrom(sipMessage.getFrom());
    }

    Sptr<BaseUrl> url = getTo().getUrl();

    SipVia sipvia;
    //create the via header. This contains details present in the toUrl.
    Sptr <BaseUrl> toUrl = getFrom().getUrl();

    //construct the request line.
    SipRequestLine siprequestline(SIP_BYE, url);
    setRequestLine(siprequestline);


    //assign the CallId.
    setCallId(sipMessage.getCallId());

    //create the CSeq header
    SipCSeq cseq = sipMessage.getCSeq();
    int value = cseq.getCSeqData().convertInt();
    //increment cseq.
    value++;

    SipCSeq newcseq(SIP_BYE, Data(value));
    setCSeq(newcseq);
    if ( dynamic_cast < const StatusMsg* > (&sipMessage) )
    {
        //construct the route list, based on the record route.
        int numRecordRoute = sipMessage.getNumRecordRoute();
        if (numRecordRoute)
        {
            for (int i = numRecordRoute - 1; i >= 0; i--)
                // for(int i = 0; i < numRecordRoute; i++)
            {
                const SipRecordRoute& recordroute = sipMessage.getRecordRoute(i);

                SipRoute route;
                route.setUrl(recordroute.getUrl());
                setRoute(route);
            }
        }

        if (sipMessage.getNumContact() == 1)
        {
            SipRoute route;
            route.setUrl(sipMessage.getContact().getUrl());
            setRoute(route);

            //remove the contact header.
            setNumContact(0);
        }
    }
    else
    {
        if ( dynamic_cast < const SipCommand* > (&sipMessage) )
        {
            //construct the route list, based on the record route.
            int numRecordRoute = sipMessage.getNumRecordRoute();
            if (numRecordRoute)
            {

                //for(int i = numRecordRoute-1; i >= 0; i--)
                for (int i = 0 ; i < numRecordRoute; i++)
                {
                    const SipRecordRoute& recordroute = sipMessage.getRecordRoute(i);
                    SipRoute route;
                    route.setUrl(recordroute.getUrl());
                    setRoute(route);
                }
            }

            if (sipMessage.getNumContact() == 1)
            {
                SipRoute route;
                route.setUrl(sipMessage.getContact().getUrl());
                setRoute(route);

                //remove the contact header.
                setNumContact(0);
            }
        }
        else
        {
            assert(0);
        }
    }

    // set up the URI
    Sptr <BaseUrl> dest;
    dest = getTo().getUrl();

    assert( getNumContact() == 0);

    int numRoute = getNumRoute();
    if (numRoute != 0 )
    {
        dest = getRoute(0).getUrl();
        //remove that route.
        removeRoute(0);
    }
    else
    {
        dest = getTo().getUrl();
    }

    setContentLength(ZERO_CONTENT_LENGTH);
    myRequestLine.setUrl(dest);

    //Set the transport of Request line.
    sipvia.setprotoVersion("2.0");
    if (toUrl.getPtr() != 0)
    {
	if (toUrl->getType() == SIP_URL)
	{
	    Sptr<SipUrl> sipUrl;
	    sipUrl.dynamicCast(toUrl);
    
	    sipvia.setHost(sipUrl->getHost());
	    sipvia.setPort(sipUrl->getPort().convertInt());
            if(myRequestLine.getTransportParam().length())
            {
                if(myRequestLine.getTransportParam() == "tcp")
                {
                    sipvia.setTransport("TCP");
                }
            }
	}
    }
    //add via to the top
    setVia(sipvia, 0);
}



ByeMsg::~ByeMsg()
{}



Method ByeMsg::getType() const
{
    return SIP_BYE;
}
