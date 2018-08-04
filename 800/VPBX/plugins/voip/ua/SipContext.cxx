/*
* $Id: SipContext.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"
#include "SipContext.hxx"
#include "SipSdp.hxx"
#include "SipRecordRoute.hxx"
#include "string.h"

using namespace Assist;


SipContext::SipContext()
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

SipContext::~SipContext()
{
	cpLog(LOG_DEBUG, "Deconstructing SipContext....." );
}

void SipContext::setOriginator( const bool yorn )
{
	originator = yorn;
}

bool SipContext::isOriginator()
{
	return originator;
}

void SipContext::setCallee( const SipUrl url )
{
	callee = url;
}

SipUrl SipContext::getCallee()
{
	return callee;
}

Sptr <Contact> SipContext::getContact()
{
	return currentContact;
}

void SipContext::setContact( Sptr <Contact> contact )
{
	currentContact = contact;
}

Sptr <ContactList> SipContext::getContactList()
{
	return contactList;
}

void SipContext::addContact( const Sptr <Contact> contact )
{
	contactList->push_back( contact );
}

void SipContext::setContactMsg( Sptr<InviteMsg> msg )
{
	Sptr <Contact> contact = new Contact( *msg );
	// Set as current contact
	setContact( contact );
	// Add to contact list
	addContact( contact );
}

void SipContext::setContactMsg( InviteMsg& msg )
{
	Sptr <Contact> contact = new Contact( msg );
	// Set as current contact
	setContact( contact );
	// Add to contact list
	addContact( contact );
}

Sptr <Contact> SipContext::findContact( const SipMsg& msg )
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


void SipContext::removeContact( const Sptr <Contact> contact )
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

void SipContext::clearContactList()
{
	contactList->clear();
}

void SipContext::setCaller( const SipUrl url )
{
	caller = url;
}

SipUrl SipContext::getCaller()
{
	return caller;
}

void SipContext::setLocalSdp( const Sptr <SipSdp> sdp )
{
	localSdp = sdp;
}

void SipContext::setRemoteSdp( const Sptr <SipSdp> sdp )
{
	remoteSdp = sdp;
}

Sptr <SipSdp> SipContext::getLocalSdp()
{
	return localSdp;
}

Sptr <SipSdp> SipContext::getRemoteSdp()
{
	return remoteSdp;
}

void SipContext::setLocal2Sdp( const Sptr <SipSdp> sdp )
{
	local2Sdp = sdp;
}

void SipContext::setRemote2Sdp( const Sptr <SipSdp> sdp )
{
	remote2Sdp = sdp;
}

Sptr <SipSdp> SipContext::getLocal2Sdp()
{
	return local2Sdp;
}

Sptr <SipSdp> SipContext::getRemote2Sdp()
{
	return remote2Sdp;
}

void SipContext::setRingInvite( Sptr <InviteMsg> invite )
{
	ringInvite = invite;
}

Sptr <InviteMsg> SipContext::getRingInvite()
{
	return ringInvite;
}

//added by lijie
void SipContext::setCalleeIdle(bool bIdle)
{
	isCalleeIdle = bIdle;
}

bool SipContext::getCalleeIdle()
{
	return isCalleeIdle;
}

void SipContext::setTimerSet(bool bSet)
{
	bTimerSet = bSet;
}

bool SipContext::getTimerSet()
{
	return bTimerSet;
}

void SipContext::setAudioSet(bool bSet)
{
	bAudioSet = bSet;
}

bool SipContext::getAudioSet()
{
	return bAudioSet;
}
//added ends here
void SipContext::setRing2Invite( Sptr <InviteMsg> invite )
{
	ring2Invite = invite;
}

Sptr <InviteMsg> SipContext::getRing2Invite()
{
	return ring2Invite;
}

void SipContext::setCallerRoute1List( SipRecordRouteList recordrouteList )
{
	route1List->clear();
	SipRecordRouteList::iterator iter = recordrouteList.begin();
	while ( iter != recordrouteList.end() )
	{
		Sptr <BaseUrl> baseUrl = (*iter)->getUrl();
		if( baseUrl->getType() == TEL_URL )
		{
			cpLog( LOG_ERR, "TEL_URL currently not supported\n" );
			assert( 0 );
		}
		// Assume we have a SIP_URL
		Sptr <SipUrl> sipUrl;
		sipUrl.dynamicCast( baseUrl );
		assert( sipUrl != 0 );

		cpDebug(LOG_DEBUG, "setCallerRoute1List %s:%s", sipUrl->getHost().logData(), sipUrl->getPort().logData() );
		Sptr <SipRoute> route = new SipRoute;
		route->setUrl( baseUrl );
		route1List->insert( route1List->begin(), route );
		iter++;
	}
}

void SipContext::setCalleeRoute1List( SipRecordRouteList recordrouteList )
{
	route1List->clear();
	SipRecordRouteList::iterator iter = recordrouteList.begin();
	while ( iter != recordrouteList.end() )
	{
		Sptr <BaseUrl> baseUrl = (*iter)->getUrl();
		if( baseUrl->getType() == TEL_URL )
		{
			cpLog( LOG_ERR, "TEL_URL currently not supported\n" );
			assert( 0 );
		}
		// Assume we have a SIP_URL
		Sptr <SipUrl> sipUrl;
		sipUrl.dynamicCast( baseUrl );
		assert( sipUrl != 0 );

		Sptr <SipRoute> route = new SipRoute;
		route->setUrl( baseUrl );

		cpDebug(LOG_DEBUG, "setCalleeRoute1List %s", route->encode().logData() );
		route1List->push_back( route );
		iter++;
	}
}

void SipContext::addRoute1( const Sptr <SipRoute> sipRoute )
{
	route1List->push_back( sipRoute );
}

vector <SipRoute*> SipContext::getRoute1List()
{
	vector <SipRoute*> siprouteList;

	RouteList::iterator iter = route1List->begin();
	while ( iter != route1List->end() )
	{
		Sptr <BaseUrl> baseUrl = (*iter)->getUrl();
		if( baseUrl->getType() == TEL_URL )
		{
			cpLog( LOG_ERR, "TEL_URL currently not supported\n" );
			assert( 0 );
		}
		// Assume we have a SIP_URL
		Sptr <SipUrl> sipUrl;
		sipUrl.dynamicCast( baseUrl );
		assert( sipUrl != 0 );

		cpDebug(LOG_DEBUG, "getRoute1List %s", sipUrl->encode().logData() );
		siprouteList.push_back( new SipRoute( *(*iter) ) );
		iter++;
	}
	return siprouteList;
}

void SipContext::setCallerRoute2List( SipRecordRouteList recordrouteList )
{
	route2List->clear();
	SipRecordRouteList::iterator iter = recordrouteList.begin();
	while ( iter != recordrouteList.end() )
	{
		Sptr <SipRoute> route = new SipRoute;
		route->setUrl( (*iter)->getUrl() );
		route2List->insert( route2List->begin(), route );
		iter++;
	}
}

void SipContext::setCalleeRoute2List( SipRecordRouteList recordrouteList )
{
	route2List->clear();
	SipRecordRouteList::iterator iter = recordrouteList.begin();
	while ( iter != recordrouteList.end() )
	{
		Sptr <SipRoute> route = new SipRoute;
		route->setUrl( (*iter)->getUrl() );
		route2List->push_back( route );
		iter++;
	}
}


void SipContext::addRoute2( const Sptr < SipRoute > sipRoute )
{
	route2List->push_back( sipRoute );
}

vector <SipRoute*> SipContext::getRoute2List()
{
	vector <SipRoute*> siprouteList;

	RouteList::iterator iter = route2List->begin();
	while ( iter != route2List->end() )
	{
		siprouteList.push_back( new SipRoute( *(*iter) ) );
		iter++;
	}
	return siprouteList;
}

void SipContext::setXferMsg( Sptr <TransferMsg> msg )
{
	xferMsg = msg;
}

Sptr <TransferMsg> SipContext::getXferMsg()
{
	return xferMsg;
}

void SipContext::setAckMsg( Sptr <AckMsg> ack )
{
	ackMsg = ack;
}


Sptr <AckMsg> SipContext::getAckMsg()
{
	return ackMsg;
}

void SipContext::setAuthorization(bool m)
{
	isAuthorization = m;
}

bool SipContext::getAuthorization() const
{
	return isAuthorization;
}

