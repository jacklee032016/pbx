/*
 * $Id: ReferMsg.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */

#include "ReferMsg.hxx"
#include "SipContact.hxx"
#include "SipProxyAuthenticate.hxx"
#include "SipReferTo.hxx"
#include "SipRequire.hxx"
#include "SipRetryAfter.hxx"
#include "SipRecordRoute.hxx"
#include "SipRoute.hxx"
#include "SipServer.hxx"
#include "SipUnsupported.hxx"
#include "SipVia.hxx"
#include "SipWarning.hxx"
#include "SipWwwAuthenticate.hxx"
#include "SystemInfo.hxx"
#include "cpLog.h"
#include "global.h"
#include "symbols.hxx"

using namespace Vocal;



ReferMsg::ReferMsg()
        : SipCommand()
{
    myRequestLine.setMethod(REFER_METHOD);
    SipCSeq cseq( SIP_REFER, 0 );
    setCSeq( cseq );
}


ReferMsg::~ReferMsg()
{
}


ReferMsg::ReferMsg(const ReferMsg& src)
        : SipCommand(src)
{
}


const ReferMsg& 
ReferMsg::operator =(const ReferMsg& src)
{
    //copy details contained in the base class.
    if ( &src != this)
    {
        *static_cast < SipCommand* > (this) = src;
    }
    return (*this);
}

    
bool 
ReferMsg::operator ==(const ReferMsg& src)
{
    return ( *static_cast <SipCommand*> (this) == src );
}
    
    
ReferMsg::ReferMsg( const StatusMsg& statusMsg, Sptr <BaseUrl> referToUrl )
    : SipCommand()
{
    assert( referToUrl != 0 );

    setTo( statusMsg.getTo() );
    setFrom( statusMsg.getFrom() );

    // Construct Route base on Record-Route
    int numRecordRoute = statusMsg.getNumRecordRoute();
    if( numRecordRoute )
    {
        for( int i=numRecordRoute-1; i >= 0; i-- )
        {
            const SipRecordRoute& rr = statusMsg.getRecordRoute( i );
            SipRoute route;
            route.setUrl( rr.getUrl() );
            setRoute( route );
        }
    }

    setReferDetails( statusMsg, referToUrl );
}


ReferMsg::ReferMsg( const SipCommand& sipCommand, Sptr <BaseUrl>  referToUrl )
{
    assert( referToUrl != 0 );

    // Swap To and From
    const SipFrom& srcFrom = sipCommand.getFrom();
    SipTo to( srcFrom.getUrl() );
    to.setDisplayName( srcFrom.getDisplayName() );
    to.setTag( srcFrom.getTag() );
    to.setToken( srcFrom.getToken() );
    to.setQstring( srcFrom.getQstring() );
    setTo( to );

    const SipTo& srcTo = sipCommand.getTo();
    SipFrom from( srcTo.getUrl() );
    from.setDisplayName( srcTo.getDisplayName() );
    from.setTag( srcTo.getTag() );
    from.setToken( srcTo.getToken() );
    from.setQstring( srcTo.getQstring() );
    setFrom( from );

    // Construct Route base on Record-Route
    int numRecordRoute = sipCommand.getNumRecordRoute();
    if( numRecordRoute )
    {
        for( int i=0; i < numRecordRoute-1; i++ )
        {
            const SipRecordRoute& rr = sipCommand.getRecordRoute( i );
            SipRoute route;
            route.setUrl( rr.getUrl() );
            setRoute( route );
        }
    }

    setReferDetails( sipCommand, referToUrl );
}


ReferMsg::ReferMsg(const ReferMsg& msg, enum ReferMsgForward)
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
    
    SipRequestLine reqLine = getRequestLine();
    reqLine.setUrl( dest );
    setRequestLine( reqLine );
}


void 
ReferMsg::setReferDetails( const SipMsg& sipMsg, Sptr <BaseUrl> referToUrl )
{
    // Put the far end Contact URI to the end of the Route List
    if( sipMsg.getNumContact() == 1 )
    {
        SipRoute route;
        route.setUrl( sipMsg.getContact().getUrl() );
        setRoute( route );
    }
    else
    {
        cpLog( LOG_ERR, "Number of Contact header (%d) is not 1",
                        sipMsg.getNumContact() );
    }

    // Remove the Contact header copied from the other message
    setNumContact( 0 );

    // Copy CallId
    setCallId( sipMsg.getCallId() );

    // Set up the Request URI
    Sptr < BaseUrl > dest = getTo().getUrl();
    if( getNumRoute() > 0 )
    {
        dest = getRoute( 0 ).getUrl();
        // Remove the first route from the Route list
        removeRoute( 0 );
    }

    // Construct request line
    SipRequestLine requestLine( SIP_REFER, dest );
    setRequestLine( requestLine );

    // Set CSeq to the next number
    SipCSeq newcseq( SIP_REFER, Data(sipMsg.getCSeq().getNextCSeq()) );
    setCSeq( newcseq );

    // Set Via header
    SipVia sipVia;
    sipVia.setprotoVersion( "2.0" );

    Sptr < BaseUrl > myUrl = getFrom().getUrl();
    if( myUrl != 0 )
    {
        Sptr<SipUrl> sipUrl;
        sipUrl.dynamicCast( myUrl );
        if( sipUrl != 0 )
        {
            sipVia.setHost( sipUrl->getHost() );
            sipVia.setPort( sipUrl->getPort().convertInt() );
            // Set transport parameter in Via
            if( myRequestLine.getTransportParam().length() )
            {
                if( myRequestLine.getTransportParam() == "tcp" )
                {
                    sipVia.setTransport( "TCP" );
                }
            }
        }
    }
    setVia( sipVia, 0 );

    // Clear content data
    setContentLength( ZERO_CONTENT_LENGTH );

    // Add Refer-To header
    SipReferTo referTo( referToUrl );
    setReferTo( referTo );

    // TODO: Is this supposed to be done in the stack?
    SipRequire require;
    require.set( "cc Refer" );
    setRequire( require );
}


ReferMsg::ReferMsg( const Data& data )
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

}    // ReferMsg::ReferMsg( Data& data )


Method 
ReferMsg::getType() const
{
    return SIP_REFER;
}
