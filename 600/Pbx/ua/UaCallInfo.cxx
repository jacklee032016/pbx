/*
 * $Id: UaCallInfo.cxx,v 1.3 2007/03/28 19:03:39 lizhijie Exp $
 */

#include "global.h"
#include "UaCallInfo.hxx"
#include "SipSdp.hxx"
#include "SipRecordRoute.hxx"
#include "string.h"

using namespace Vocal;


UaCallInfo::UaCallInfo()
{
    originator = false;
    contactList = new ContactList;
    route1List = new RouteList;
    route2List = new RouteList;
    xferMsg = 0;
    ackMsg = 0;
    ringInvite = 0;
    ring2Invite = 0;
    currentContact = 0;
    localSdp = 0;
    remoteSdp = 0;
    local2Sdp = 0;
    remote2Sdp = 0;
    isAuthorization = true;
    bTimerSet = false;
    isCalleeIdle = false;
    bAudioSet = false;	
}


UaCallInfo::~UaCallInfo()
{
}


void
UaCallInfo::setOriginator( const bool yorn )
{
    originator = yorn;
}


bool
UaCallInfo::isOriginator()
{
    return originator;
}


void
UaCallInfo::setCallee( const SipUrl url )
{
    callee = url;
}


SipUrl
UaCallInfo::getCallee()
{
    return callee;
}


Sptr < Contact >
UaCallInfo::getContact()
{
    return currentContact;
}


void
UaCallInfo::setContact( Sptr < Contact > contact )
{
    currentContact = contact;
}


Sptr < ContactList >
UaCallInfo::getContactList()
{
    return contactList;
}


void
UaCallInfo::addContact( const Sptr < Contact > contact )
{
    contactList->push_back( contact );
}


void
UaCallInfo::setContactMsg( Sptr<InviteMsg> msg )
{
    Sptr < Contact > contact = new Contact( *msg );
    // Set as current contact
    setContact( contact );
    // Add to contact list
    addContact( contact );
}


void
UaCallInfo::setContactMsg( InviteMsg& msg )
{
    Sptr < Contact > contact = new Contact( msg );
    // Set as current contact
    setContact( contact );
    // Add to contact list
    addContact( contact );
}


Sptr < Contact >
UaCallInfo::findContact( const SipMsg& msg )
{
#if 0
    ContactList::iterator iter = contactList->begin();
    while ( iter != contactList->end() )
    {
        if ( (*(*iter)).computeCallLeg() == msg.computeCallLeg() )
        {
            return (*iter);
        }
        iter++;
    }
#else
    ContactList::iterator iter = contactList->end();
    while ( iter != contactList->begin() )
    {
        iter--;
        if ( (*(*iter)).computeCallLeg() == msg.computeCallLeg() )
        {
            return (*iter);
        }
    }
#endif
    return 0;
}


void
UaCallInfo::removeContact( const Sptr < Contact > contact )
{
    ContactList::iterator iter = contactList->begin();
    while ( iter != contactList->end() )
    {
        if ( *(*iter) == *contact )
        {
            contactList->erase(iter);
            break;
        }
        iter++;
    }

    return ;
}


void
UaCallInfo::clearContactList()
{
    contactList->clear();
}


void
UaCallInfo::setCaller( const SipUrl url )
{
    caller = url;
}


SipUrl
UaCallInfo::getCaller()
{
    return caller;
}


void
UaCallInfo::setLocalSdp( const Sptr < SipSdp > sdp )
{
    localSdp = sdp;
}


void
UaCallInfo::setRemoteSdp( const Sptr < SipSdp > sdp )
{
    remoteSdp = sdp;
}


Sptr < SipSdp >
UaCallInfo::getLocalSdp()
{
    return localSdp;
}


Sptr < SipSdp >
UaCallInfo::getRemoteSdp()
{
    return remoteSdp;
}


void
UaCallInfo::setLocal2Sdp( const Sptr < SipSdp > sdp )
{
    local2Sdp = sdp;
}


void
UaCallInfo::setRemote2Sdp( const Sptr < SipSdp > sdp )
{
    remote2Sdp = sdp;
}


Sptr < SipSdp >
UaCallInfo::getLocal2Sdp()
{
    return local2Sdp;
}


Sptr < SipSdp >
UaCallInfo::getRemote2Sdp()
{
    return remote2Sdp;
}


void
UaCallInfo::setRingInvite( Sptr < InviteMsg > invite )
{
    ringInvite = invite;
}


Sptr < InviteMsg >
UaCallInfo::getRingInvite()
{
    return ringInvite;
}

//added by lijie
  void UaCallInfo::setCalleeIdle(bool bIdle)
 {
 	isCalleeIdle = bIdle;
 }
 bool UaCallInfo::getCalleeIdle()
 {
 	return isCalleeIdle;
 }

  void UaCallInfo::setTimerSet(bool bSet)
 {
 	bTimerSet = bSet;
 }

 bool UaCallInfo::getTimerSet()
 {
 	return bTimerSet;
 }
   void UaCallInfo::setAudioSet(bool bSet)
 {
 	bAudioSet = bSet;
 }

 bool UaCallInfo::getAudioSet()
 {
 	return bAudioSet;
 }

//added ends here
void
UaCallInfo::setRing2Invite( Sptr < InviteMsg > invite )
{
    ring2Invite = invite;
}


Sptr < InviteMsg >
UaCallInfo::getRing2Invite()
{
    return ring2Invite;
}


void
UaCallInfo::setCallerRoute1List( SipRecordRouteList recordrouteList )
{
    route1List->clear();
    SipRecordRouteList::iterator iter = recordrouteList.begin();
    while ( iter != recordrouteList.end() )
    {
	Sptr< BaseUrl > baseUrl = (*iter)->getUrl();
	if( baseUrl->getType() == TEL_URL )
	{
	    cpLog( LOG_ERR, "TEL_URL currently not supported\n" );
	    assert( 0 );
	}
	// Assume we have a SIP_URL
	Sptr< SipUrl > sipUrl;
	sipUrl.dynamicCast( baseUrl );
	assert( sipUrl != 0 );

        cpDebug(LOG_DEBUG, "setCallerRoute1List %s:%s",
	      sipUrl->getHost().logData(),
	      sipUrl->getPort().logData() );
        Sptr < SipRoute > route = new SipRoute;
        route->setUrl( baseUrl );
        route1List->insert( route1List->begin(), route );
        iter++;
    }
}


void
UaCallInfo::setCalleeRoute1List( SipRecordRouteList recordrouteList )
{
    route1List->clear();
    SipRecordRouteList::iterator iter = recordrouteList.begin();
    while ( iter != recordrouteList.end() )
    {
	Sptr< BaseUrl > baseUrl = (*iter)->getUrl();
	if( baseUrl->getType() == TEL_URL )
	{
	    cpLog( LOG_ERR, "TEL_URL currently not supported\n" );
	    assert( 0 );
	}
	// Assume we have a SIP_URL
	Sptr< SipUrl > sipUrl;
	sipUrl.dynamicCast( baseUrl );
	assert( sipUrl != 0 );

        Sptr< SipRoute > route = new SipRoute;
        route->setUrl( baseUrl );

        cpDebug(LOG_DEBUG, "setCalleeRoute1List %s", route->encode().logData() );

        route1List->push_back( route );
        iter++;
    }
}


void
UaCallInfo::addRoute1( const Sptr < SipRoute > sipRoute )
{
    route1List->push_back( sipRoute );
}


vector < SipRoute* >
UaCallInfo::getRoute1List()
{
    vector < SipRoute* > siprouteList;

    RouteList::iterator iter = route1List->begin();

    while ( iter != route1List->end() )
    {
	Sptr< BaseUrl > baseUrl = (*iter)->getUrl();
	if( baseUrl->getType() == TEL_URL )
	{
	    cpLog( LOG_ERR, "TEL_URL currently not supported\n" );
	    assert( 0 );
	}
	// Assume we have a SIP_URL
	Sptr< SipUrl > sipUrl;
	sipUrl.dynamicCast( baseUrl );
	assert( sipUrl != 0 );

        cpDebug(LOG_DEBUG, "getRoute1List %s", sipUrl->encode().logData() );

        siprouteList.push_back( new SipRoute( *(*iter) ) );
        iter++;
    }
    return siprouteList;
}


void
UaCallInfo::setCallerRoute2List( SipRecordRouteList recordrouteList )
{
    route2List->clear();
    SipRecordRouteList::iterator iter = recordrouteList.begin();
    while ( iter != recordrouteList.end() )
    {
        Sptr < SipRoute > route = new SipRoute;
        route->setUrl( (*iter)->getUrl() );
        route2List->insert( route2List->begin(), route );
        iter++;
    }
}


void
UaCallInfo::setCalleeRoute2List( SipRecordRouteList recordrouteList )
{
    route2List->clear();
    SipRecordRouteList::iterator iter = recordrouteList.begin();
    while ( iter != recordrouteList.end() )
    {
        Sptr < SipRoute > route = new SipRoute;
        route->setUrl( (*iter)->getUrl() );
        route2List->push_back( route );
        iter++;
    }
}


void
UaCallInfo::addRoute2( const Sptr < SipRoute > sipRoute )
{
    route2List->push_back( sipRoute );
}


vector < SipRoute* >
UaCallInfo::getRoute2List()
{
    vector < SipRoute* > siprouteList;

    RouteList::iterator iter = route2List->begin();

    while ( iter != route2List->end() )
    {
        siprouteList.push_back( new SipRoute( *(*iter) ) );
        iter++;
    }
    return siprouteList;
}


void
UaCallInfo::setXferMsg( Sptr < TransferMsg > msg )
{
    xferMsg = msg;
}


Sptr < TransferMsg >
UaCallInfo::getXferMsg()
{
    return xferMsg;
}


void
UaCallInfo::setAckMsg( Sptr < AckMsg > ack )
{
    ackMsg = ack;
}


Sptr < AckMsg >
UaCallInfo::getAckMsg()
{
    return ackMsg;
}


/* added by wuaiwu, 2005/07/07 */
void UaCallInfo::setAuthorization(bool m)
{
	isAuthorization = m;
}
bool UaCallInfo::getAuthorization() const
{
	return isAuthorization;
}

