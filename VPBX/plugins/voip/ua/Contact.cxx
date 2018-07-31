/*
* $Id: Contact.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "global.h"
#include "SipUrl.hxx"
#include "Contact.hxx"

using namespace Assist;

Contact::Contact( const Contact& c )
        : inviteMsg( c.inviteMsg ), 
        statusMsg( c.statusMsg )
{
	status = c.status;
	cSeqNum = c.cSeqNum;
	lastInfoCSeq = c.lastInfoCSeq;
	remoteRingback = false;
}

Contact::Contact( const InviteMsg& aMsg )
        : inviteMsg( aMsg )
{
	status = 0;
	lastInfoCSeq = 0;
	cSeqNum = inviteMsg.getCSeq().getCSeqData().convertInt();
	remoteRingback = false;
}

void Contact::update( const StatusMsg& msg )
{
    //for now, we will only update the status member of the class
    //I am sure this is incomplete.  Will come back to it later on.
	status = msg.getStatusLine().getStatusCode();
	statusMsg = msg;
}

int Contact::getBranch()
{
	return 1;
}

int Contact::getStatus()
{
	return status;
}

bool Contact::operator==( const Contact& other ) const
{
	return ( computeCallLeg() == other.computeCallLeg() );
}

bool Contact::operator==( const SipMsg& msg )
{
	return ( computeCallLeg() == msg.computeCallLeg() );
}

bool Contact::operator!=( const Contact& other )
{
	return !( *this == other );
}

void Contact::operator = (const Contact& src)
{
	inviteMsg = src.inviteMsg;
	statusMsg = src.statusMsg;
	status = src.status;
	cSeqNum = src.cSeqNum;
	lastInfoCSeq = src.lastInfoCSeq;
	remoteRingback = src.remoteRingback;
}

Sptr  <BaseUrl> Contact::getLocation()
{
	return inviteMsg.getRequestLine().getUrl();
}

InviteMsg Contact::getInviteMsg()
{
	return inviteMsg;
}

StatusMsg Contact::getStatusMsg()
{
	return statusMsg;
}

SipCallLeg Contact::computeCallLeg() const
{
	return inviteMsg.computeCallLeg();
}

void Contact::setLastInfoCSeq( unsigned int cseq )
{
	lastInfoCSeq = cseq;
}

unsigned int Contact::getLastInfoCSeq()
{
	return lastInfoCSeq;
}

void Contact::setCSeqNum( unsigned int cseq )
{
	cSeqNum = cseq;
}

void Contact::setRemoteRingback( bool flag )
{
	remoteRingback = flag;
}

bool Contact::getRemoteRingback() const
{
	return remoteRingback;
}

