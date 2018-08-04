/*

* $Id: AckMsg.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"
#include <cstdlib>
#include <cstdio>

#include "AckMsg.hxx"
#include "SipCommand.hxx"
#include "SipContact.hxx"
#include "SipMsg.hxx"
#include "SipProxyAuthenticate.hxx"
#include "SipProxyRequire.hxx"
#include "SipRequire.hxx"
#include "SipRoute.hxx"
#include "SipRecordRoute.hxx"
#include "SipUnsupported.hxx"
#include "SipVia.hxx"
#include "SipWarning.hxx"
#include "SipWwwAuthenticate.hxx"
#include "StatusMsg.hxx"
#include "SystemInfo.hxx"

using namespace Assist;


AckMsg::AckMsg(const AckMsg& src)
        : SipCommand(src)
{
}


AckMsg::AckMsg(const Data& data)
        : SipCommand()
{
    SipCommand::decode(data);
}


AckMsg::AckMsg()
        : SipCommand()
{
    myRequestLine.setMethod(ACK_METHOD);
    SipCSeq cseq( SIP_ACK, 0 );
    setCSeq( cseq );
}


AckMsg::AckMsg(const SipCommand& src, const SipVia& via, const SipCSeq& cseq)
        : SipCommand(src, via, cseq)
{
    myRequestLine.setMethod(ACK_METHOD);
}


AckMsg::AckMsg(StatusMsg& statusMsg)
        : SipCommand(statusMsg)
{
    myRequestLine.setMethod(ACK_METHOD);
    if (!statusMsg.viaEmpty())
    {
        viaPushFront(statusMsg.viaFront());
    }
    
    Data data(statusMsg.getCSeq().getCSeqData());
    SipCSeq cseq(SIP_ACK, data);
    setCSeq(cseq);
}


AckMsg::~AckMsg()
{
}


AckMsg& 
AckMsg::operator =(const AckMsg& src)
{
    if ( &src != this)
    {
	SipCommand::operator=( src );
    }
    return (*this);
}


bool 
AckMsg::operator ==(const AckMsg& src) 
{
   
   return (  *static_cast < SipCommand* > (this) == src);
   
}


AckMsg::AckMsg(const AckMsg& msg, enum AckMsgForward)
    : SipCommand(msg)
{
    Sptr<BaseUrl> dest = getTo().getUrl();

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

    myRequestLine.setUrl(dest);
}

// to be deleted
void
AckMsg::setAckDetails(const StatusMsg& statusMsg)
{

    //only the CSeq will change.

    //the Via is set to the last one.(which would contain the
    //originating client details
    assert( statusMsg.getNumVia() > 0 );


    // get the top most via, since this holds good for both 200 and
    // other > 200 final responses.

    const SipVia& via = statusMsg.getVia(0);
    setVia(via);

    // construct the To field
    const SipTo& to = statusMsg.getTo();
    setTo(to);

    Sptr<BaseUrl> url = to.getUrl();

    //construct the request line.
    SipRequestLine siprequestline(SIP_ACK, url);
    setRequestLine(siprequestline);

    //construct the From field
    setFrom(statusMsg.getFrom());

    //construct the CallId field
    setCallId(statusMsg.getCallId());

    //construct the CSeq field
    const SipCSeq& cseq = statusMsg.getCSeq();

    Data strval(cseq.getCSeqData());

    SipCSeq newcseq(SIP_ACK, strval);

    setCSeq(newcseq);

    // construct the route list, based on the record route.  For
    // status code 400 and more ignore the Record route list

    int numRecordRoute = 0;
    if(statusMsg.getStatusLine().getStatusCode()  == 200)
    {
        numRecordRoute = statusMsg.getNumRecordRoute();
    }
    SipRoute route;

    if (numRecordRoute)
    {
        for (int i = numRecordRoute - 1; i >= 0 ; i--)
        {
            const SipRecordRoute& recordroute = statusMsg.getRecordRoute(i);
            route.setUrl(recordroute.getUrl());
            setRoute(route);  // to beginning
        }

    }

    // if the status msg contains a contact header, then, copy that to
    // the route. There can be only one contact field in the response
    // (200) the contact field is left alone.
    int numContact = statusMsg.getNumContact();
    cpDebug(LOG_DEBUG_STACK, "add %d contacts to route", numContact );
    if (numContact > 0)
    {
        // contact present.  get the last contact. This is the one
        // inserted by the originating client.
        const SipContact& contact = statusMsg.getContact(numContact - 1);
        route.setUrl(contact.getUrl());
        setRoute(route);  // to beginning
        setNumContact(0);
    }

    // figure out the URI
    Sptr<BaseUrl> dest;
    dest = getTo().getUrl();

    assert( getNumContact() == 0);

    int numRoute = getNumRoute();
    //For bis-9 compliance this should be changed and should check if the first route is 
    //a loose route, if so 
    //the request-line should be the contact instead
    if (numRoute != 0)
    {
        dest = getRoute(0).getUrl();
        // remove that route.
        removeRoute(0);
    }

    myRequestLine.setUrl( dest);
    setContentLength(ZERO_CONTENT_LENGTH);
}


Method AckMsg::getType() const
{
    return SIP_ACK;
}

