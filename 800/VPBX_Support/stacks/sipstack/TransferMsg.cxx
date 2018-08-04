/*
* $Log: TransferMsg.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:10  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: TransferMsg.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include <cstdlib>
#include <cstdio>
#include "Data.hxx"
#include "cpLog.h"

#include "symbols.hxx"
#include "SystemInfo.hxx"

#include "TransferMsg.hxx"
#include "SipVia.hxx"
#include "SipContact.hxx"
#include "SipProxyAuthenticate.hxx"
#include "SipRequire.hxx"
#include "SipRetryAfter.hxx"
#include "SipRecordRoute.hxx"
#include "SipRoute.hxx"
#include "SipServer.hxx"
#include "SipUnsupported.hxx"
#include "SipWarning.hxx"
#include "SipWwwAuthenticate.hxx"
#include "SipTransferTo.hxx"

using namespace Assist;


TransferMsg::TransferMsg()
        : SipCommand()
{}



TransferMsg::TransferMsg(const TransferMsg& src)
        : SipCommand(src)
{
}


TransferMsg& TransferMsg::operator =(const TransferMsg& src)
{
    if ( &src != this)
    {
        //copy details contained in the base class.
        *static_cast < SipCommand* > (this) = src;
    }
    return (*this);
}

TransferMsg::TransferMsg(const StatusMsg& statusMsg, Sptr <BaseUrl> transferToUrl)
        : SipCommand()
{
    cpDebug(LOG_DEBUG_STACK, "C'tor of Transfer");

    if (transferToUrl.getPtr() != 0)
    {
	cpDebug(LOG_DEBUG_STACK, "SipUrl: %s", transferToUrl->encode().logData());
    }
    int type = statusMsg.getType();

    setTransferDetails(statusMsg, type, transferToUrl);
}

TransferMsg::TransferMsg(const SipCommand& sipCommand, Sptr <BaseUrl> transferToUrl)
{
    cpDebug(LOG_DEBUG_STACK, "C'tor of Transfer");

    if (transferToUrl.getPtr() != 0)
    {
	cpDebug(LOG_DEBUG_STACK, "SipUrl: %s", transferToUrl->encode().logData());
    }
    int type = sipCommand.getType();

    setTransferDetails(sipCommand, type, transferToUrl);
}

TransferMsg::TransferMsg(const TransferMsg& msg, enum TransferMsgForward)
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

void TransferMsg::setTransferDetails(const SipMsg& sipMsg,
                                     int type,
                                     Sptr <BaseUrl> transferToUrl)
{
    if (transferToUrl.getPtr() != 0)
    {
	cpDebug(LOG_DEBUG_STACK, "SipUrl:setTransferDetails: %s",
	      transferToUrl->encode().logData());
    }



    if (type == SIP_INVITE)
    {
        SipFrom srcfrom = sipMsg.getFrom();
        SipTo srcto = sipMsg.getTo();
        //set the To header.
        SipTo to(srcfrom.getUrl());
        setTo(to);

        //set the From header.
        SipTo srcTo = sipMsg.getTo();
        SipFrom from(srcTo.getUrl());
        setFrom(from);

        //construct the route list, based on the record route.
        int numRecordRoute = sipMsg.getNumRecordRoute();
        if (numRecordRoute)
        {
            for (int i = 0 ; i < numRecordRoute; i++)
            {
                SipRecordRoute recordroute = sipMsg.getRecordRoute(i);

                SipRoute route;
                route.setUrl(recordroute.getUrl());
                setRoute(route);
            }
        }

        if (sipMsg.getNumContact() == 1)
        {
            SipRoute route;
            route.setUrl(sipMsg.getContact().getUrl());
            setRoute(route);

            //remove the contact header.
            setNumContact(0);
        }
    }

    else if (type == SIP_STATUS)
    {
        setTo(sipMsg.getTo());
        setFrom(sipMsg.getFrom());

        //construct the route list, based on the record route.
        int numRecordRoute = sipMsg.getNumRecordRoute();
        if (numRecordRoute)
        {
            for (int i = numRecordRoute - 1; i >= 0; i--)
            {
                SipRecordRoute recordroute = sipMsg.getRecordRoute(i);

                SipRoute route;
                route.setUrl(recordroute.getUrl());
                setRoute(route);
            }
        }

        if (sipMsg.getNumContact() == 1)
        {
            SipRoute route;
            route.setUrl(sipMsg.getContact().getUrl());
            setRoute(route);

            //remove the contact header.
            setNumContact(0);
        }
    }

    Sptr <BaseUrl> url = getTo().getUrl();

    SipVia sipvia;
    //create the  via header. This contains details present in the toUrl.
    Sptr<BaseUrl> toUrl = getFrom().getUrl();

    //default transport is UDP.
    sipvia.setprotoVersion("2.0");
    if (toUrl.getPtr() != 0)
    {
	if (toUrl->getType() == SIP_URL)
	{
	    Sptr <SipUrl> sipUrl;
	    sipUrl.dynamicCast(toUrl);
	    sipvia.setHost(sipUrl->getHost());
	    sipvia.setPort(sipUrl->getPort().convertInt());
	}
    }

    //construct the request line.
    SipRequestLine siprequestline(SIP_TRANSFER, url);
    setRequestLine(siprequestline);

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

    SipRequestLine reqLine = getRequestLine();
    reqLine.setUrl( dest );
    setRequestLine( reqLine );



    //add via to the top
    setVia(sipvia, 0);

    //assign the CallId.
    setCallId(sipMsg.getCallId());

    //create the CSeq header
    SipCSeq cseq = sipMsg.getCSeq();
    int value = cseq.getCSeqData().convertInt();
    //increment cseq.
    value++;

    SipCSeq newcseq(SIP_TRANSFER, Data(value));
    setCSeq(newcseq);
    //build each header , and store it in that header class.

    //create the callId header
    SipCallId callId;
    setCallId(sipMsg.getCallId());

    cpDebug(LOG_DEBUG_STACK, "Completed the call id header");


    SipTransferTo transferTo(transferToUrl);
    setTransferTo(transferTo);

    SipRequire require;
    require.set("cc Transfer");
    setRequire(require);

}


TransferMsg::TransferMsg( Data& data )
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

}    // TransferMsg::TransferMsg( Data& data )


Method 
TransferMsg::getType() const
{
    return SIP_TRANSFER;
}
