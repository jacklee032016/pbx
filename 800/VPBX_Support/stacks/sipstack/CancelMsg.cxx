/*
* $Log: CancelMsg.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: CancelMsg.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/


#include "CancelMsg.hxx"
#include "Data.hxx"
#include "SipCallId.hxx"
#include "SipRequire.hxx"
#include "SipRoute.hxx"
#include "SipVia.hxx"

using namespace Assist;

CancelMsg::CancelMsg(const SipCommand& src)
    : SipCommand()
{
    myRequestLine = src.getRequestLine();
    myRequestLine.setMethod(CANCEL_METHOD);

    copyHeader(src, SIP_TO_HDR);
    copyHeader(src, SIP_FROM_HDR);
    copyHeader(src, SIP_CALLID_HDR);
    copyHeader(src, SIP_PROXY_AUTHORIZATION_HDR);
    copyHeader(src, SIP_ROUTE_HDR);
    
    SipCSeq cseq = src.getCSeq();
    cseq.setMethod(CANCEL_METHOD);
    setCSeq(cseq);

    viaPushFront(src.viaFront());
}


CancelMsg& 
CancelMsg::operator =(const CancelMsg& src)
{
    if ( &src != this)
    {
        *static_cast < SipCommand* > (this) = src;
    }
    return (*this);
}


CancelMsg::CancelMsg() : SipCommand()
{
    myRequestLine.setMethod(CANCEL_METHOD);
    SipCSeq cseq( SIP_CANCEL, 0 );
    setCSeq( cseq );
}


CancelMsg::CancelMsg(const CancelMsg& src)
        : SipCommand(src)
{
}

CancelMsg::CancelMsg(const Data& data)
{
    SipCommand::decode(data);

}


CancelMsg::CancelMsg(const CancelMsg& msg, enum CancelMsgForward)
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


CancelMsg::~CancelMsg()
{
}


bool 
CancelMsg::operator ==(const CancelMsg& src) 
{
   
   return (  *static_cast < SipCommand* > (this) == src);
   
}


/*** The SipCommand must be the Request which we are cancelling */
void 
CancelMsg::setCancelDetails(const SipCommand& sipCommand)
{
    //copy RequestLine ,CallId, From, To,  fields from sipCommand.
    SipRequestLine srcrequestLine;
    srcrequestLine = sipCommand.getRequestLine();
    Sptr <BaseUrl> url = srcrequestLine.getUrl();

    SipRequestLine requestLine(SIP_CANCEL, url);

    setRequestLine(requestLine);

    setCallId(sipCommand.getCallId());
    setFrom(sipCommand.getFrom());
    setTo(sipCommand.getTo());

    SipCSeq cseq = sipCommand.getCSeq();
    //change the method to CANCEL.
    Data method = CANCEL_METHOD;

    setProxyAuthorization(sipCommand.getProxyAuthorization());

    cseq.setMethod(method);
    setCSeq(cseq);

    //copy via details from invite msg.
    copyViaList(sipCommand);

    setContentLength(ZERO_CONTENT_LENGTH);
}


Method 
CancelMsg::getType() const
{
    return SIP_CANCEL;
}
