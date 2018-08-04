
#include "NotifyMsg.hxx"
#include "SipCallLeg.hxx"
#include "SipMsg.hxx"
#include "SipRoute.hxx"
#include "SipSubsNotifyEvent.hxx"
#include "SipVia.hxx"
#include "SubscribeMsg.hxx"
#include "SystemInfo.hxx"
#include "cpLog.h"
#include "symbols.hxx"
#include "SipUnknownContentData.hxx"

using namespace Assist;


NotifyMsg::NotifyMsg()
        : SipCommand()
{
    myRequestLine.setMethod(NOTIFY_METHOD);
    SipCSeq cseq( SIP_NOTIFY, 0 );
    setCSeq( cseq );
}


NotifyMsg::NotifyMsg(const Data& data)
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

NotifyMsg::NotifyMsg(const SubscribeMsg& subscribeRequest, 
		     const SipSubsNotifyEvent& event)
        : SipCommand()
{
    //set call-id, from, and to
    setCallId(subscribeRequest.getCallId());
    SipFrom newFrom(subscribeRequest.getTo());
    SipTo newTo(subscribeRequest.getFrom());
    newFrom.setTag(subscribeRequest.getTo().getTag());
    newTo.setTag(subscribeRequest.getFrom().getTag());
    setTo(newTo);
    setFrom(newFrom);

    //make sure no SubNotifyEvent headers exist in the new NOTIFY message
    flushSubsNotifyEvent();

    bool exists = false;
    for(int i = 0; i < subscribeRequest.getNumSubsNotifyEvent(); i++)
    {
	if(event == subscribeRequest.getSubsNotifyEvent(i))
	{
	    exists = true;
	}
    }
    if(exists)
    {
        setSubsNotifyEvent(event);
    }

    //set the other headers of NOTIFY message
    Sptr <BaseUrl> fromUrl = subscribeRequest.getFrom().getUrl();
    setNotifyDetails(fromUrl);
}


NotifyMsg::NotifyMsg(const SipCallLeg& callLeg, const SipSubsNotifyEvent& event)
        : SipCommand()
{
    //set call-id, from, and to
    setCallId(callLeg.getCallId());
    setTo(callLeg.getTo());
    setFrom(callLeg.getFrom());

    //make sure no SubNotifyEvent headers exist in the new NOTIFY message
    flushSubsNotifyEvent();
    setSubsNotifyEvent(event);

    //set the other headers of NOTIFY message
    Sptr <BaseUrl> fromUrl = callLeg.getFrom().getUrl();
    setNotifyDetails(fromUrl);
}


NotifyMsg::NotifyMsg(const NotifyMsg& src)
        : SipCommand(src)
{
}


NotifyMsg::~NotifyMsg()
{
}


const NotifyMsg& 
NotifyMsg::operator=(const NotifyMsg& src)
{
    if ( &src != this )
    {
        SipCommand::operator=(src);
    }
    return (*this);
}
    
    
bool 
NotifyMsg::operator ==(const NotifyMsg& src)
{
    return ( *static_cast <SipCommand*> (this) == src );
}
        

Assist::Method 
NotifyMsg::getType() const
{
    return SIP_NOTIFY;
}

SipStatusLine
NotifyMsg::getReferredStatusLine() const
{
    if (getNumSubsNotifyEvent() == 1)
    {
        Data nevent = getSubsNotifyEvent(0).getEventType();
        Data result = nevent.matchChar(";");
        
        if (result == "refer" || nevent == "refer")
        {
            Sptr<SipContentData> content = getContentData(0);
            Sptr<SipUnknownContentData> text;
            text.dynamicCast(content);
            if (text != 0)
            {
                int i;
                cpDebug(LOG_DEBUG_STACK, "output body: <%s> status=<%s>", 
                      text->encodeBody(i).logData(),
                      text->encodeBody(i).getLine().logData());
                Data frag = text->encodeBody(i);
                // !jf! this will not work if fragment contains anything other than
                // status line
                return SipStatusLine(frag);
            }
            else
            {
                cpDebug(LOG_DEBUG, "no reffered status line in %s", encode().logData());
                throw NotReferException(__FILE__, __LINE__);
            }
        }
        else
        {
            cpDebug(LOG_DEBUG, "no reffered status line in %s", encode().logData());
            throw NotReferException(__FILE__, __LINE__);
        }
    }
    return SipStatusLine();
}

NotifyMsg::NotifyMsg(const NotifyMsg& msg, enum NotifyMsgForward)
{
    *this = msg;

    Sptr <BaseUrl> dest = getTo().getUrl();

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
   //  myRequestLine.setUrl(dest);
//     SipRequestLine reqLine = getRequestLine();
//     reqLine.setUrl( dest );
//     setRequestLine( reqLine );
    myRequestLine.setUrl(dest);
}


void 
NotifyMsg::setNotifyDetails(Sptr <BaseUrl>  fromUrl)
{
    //set the CSeq
    SipCSeq cseq(Assist::SIP_NOTIFY, Data(1));
    setCSeq(cseq);

    //set the request line
    SipRequestLine requestLine(SIP_NOTIFY, fromUrl);
    setRequestLine(requestLine);

	Data tmph,hip;
	if(fromUrl.getPtr()!=0)
	{
		 tmph=requestLine.getHost();
		 hip=theSystem.gethostAddress(&tmph);
	}
	else
		hip=theSystem.gethostAddress();

    //set the Via from host creating NOTIFY msg
    SipVia sipVia;
    sipVia.setprotoVersion("2.0");
    sipVia.setHost(hip );
    setVia(sipVia);

}
