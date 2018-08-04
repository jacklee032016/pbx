/*
 * $Id: SipEvent.cxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "global.h"
#include "cpLog.h"
#include "SipEvent.hxx"
#include "InviteMsg.hxx"
#include "SipVia.hxx"
#include "CallContainer.hxx"
#include "SipTransceiver.hxx"
#include "SipMsg.hxx"

using namespace Vocal;

SipEvent::SipEvent( const Sptr < Fifo < Sptr < SipProxyEvent > > > outputFifo )
    :   SipProxyEvent(outputFifo),
        mySipMsg(0),
        mySipMsgQueue(0),
        mySipCallLeg(0)
{
}


SipEvent::~SipEvent()
{
}


void
SipEvent::setSipMsg( const Sptr < SipMsg > sipMsg )
{
    assert( sipMsg != 0 );

    mySipMsg = sipMsg;
}


const Sptr < SipMsg >
SipEvent::getSipMsg() const
{
    assert( mySipMsg != 0 );

    return ( mySipMsg );
}



void
SipEvent::setSipMsgQueue(const Sptr < SipMsgQueue > sipRcv)
{
    assert( sipRcv != 0 );

    mySipMsgQueue = sipRcv;

    setSipMsg(sipRcv->back());
}


const Sptr < SipMsgQueue >
SipEvent::getSipMsgQueue() const
{
    assert( mySipMsgQueue != 0 );

    return ( mySipMsgQueue );
}


inline void
SipEvent::setSipReceive( const Sptr < SipMsgQueue > sipRcv )
{
    setSipMsgQueue(sipRcv);
}


const Sptr < SipMsgQueue >
SipEvent::getSipReceive() const
{
    assert ( mySipMsgQueue != 0 );

    return ( mySipMsgQueue );
}


const Sptr < InviteMsg >
SipEvent::getInvite() const
{
    // iterate through all msgs associated with this event looking
    // for an INVITE, and return it if found
    Sptr < SipMsg > sipMsg;

    cpDebug(LOG_DEBUG, "Search transaction for previous INVITE" );
    Sptr < SipMsgQueue > transaction = mySipMsgQueue;
    if ( transaction != 0 )
    {
        SipMsgQueue::iterator i;
        for (i = transaction->begin(); i != transaction->end(); ++i)
        {
            sipMsg = *i;
            if ( sipMsg == 0 )
            {
                // reach empty sip msg without finding INVITE, return 0
                cpDebug(LOG_DEBUG, "no INVITE found, returning 0" );
                return 0;
            }

            if ( sipMsg->getType() == SIP_INVITE )
            {
	        Sptr < InviteMsg > invite;
                invite.dynamicCast( sipMsg );
                cpDebug(LOG_DEBUG, "Found INVITE:" );
                cpDebug(LOG_DEBUG, "%s", invite->encode().logData() );
                return invite;
            }
            cpDebug(LOG_DEBUG, "Not an INVITE:" );
            cpDebug(LOG_DEBUG, "%s", sipMsg->encode().logData() );
        }
    }

    // no Invite found
    cpDebug(LOG_DEBUG, "no INVITE found, returning 0" );
    return 0;
}


const Sptr < SipCommand >
SipEvent::getCommand() const
{
    // iterate through all msgs associated with this event looking
    // for a command, and return it if found
    Sptr < SipMsg > sipMsg;

    cpDebug(LOG_DEBUG, "Search transaction for previous command" );
    Sptr < SipMsgQueue > transaction = mySipMsgQueue;
    if ( transaction != 0 )
    {
        SipMsgQueue::iterator i;
        for (i = transaction->begin(); i != transaction->end(); ++i)
        {
            sipMsg = *i;
            if ( sipMsg == 0 )
            {
                // reach empty sip msg without finding INVITE, return 0
                cpDebug(LOG_DEBUG, "no command found, returning 0" );
                return 0;
            }

	    Sptr < SipCommand > command;
            command.dynamicCast( sipMsg );
            if ( command != 0 )
            {
                cpDebug(LOG_DEBUG, "Found command sent to %s:%d",
		                  command->getSendAddress().getIpName().c_str(),
		                  command->getSendAddress().getPort() );
                cpDebug(LOG_DEBUG, "%s", command->encode().logData() );
                return command;
            }
            cpDebug(LOG_DEBUG, "Not an INVITE:" );
            cpDebug(LOG_DEBUG, "%s", sipMsg->encode().logData() );
        }
    }

    // no command found
    cpDebug(LOG_DEBUG, "no command found, returning 0" );
    return 0;
}


const Sptr < SipCommand >
SipEvent::getPendingCommand( Sptr < SipCommand > sipCommand ) const
{
    // allocate vector to store previous msgs with same call leg
    CallLegVector callLegMsgs( mySipStack->getCallLegMsgs( sipCommand ) );

    // create iterator to search for matching commandw
    vector < Sptr < SipMsg > > ::iterator iter( callLegMsgs.begin() );

    // search through previous msgs, and return matching command if found;
    // otherwise return 0
    cpDebug(LOG_INFO, "Check each matching command" );
    Sptr < SipCommand > pendingCommand;

    while ( iter != callLegMsgs.end() )
    {
        cpDebug(LOG_INFO, "Check this command" );

        if ( *iter == 0 )
        {
            cpLog( LOG_ERR, "This command is empty" );
        }

        else if ( pendingCommand.dynamicCast( *iter ) == 0 )
        {
            cpLog( LOG_ERR, "This command is not a SIP command" );
        }

        else
        {
            // first Via of Command must match second Via of pending command
            if ( ( pendingCommand->getNumVia() >= 2 ) &&
                    ( sipCommand->getNumVia() >= 1 ) )
            {
                if ( pendingCommand->getVia( 1 ) ==
                        sipCommand->getVia( 0 ) )
                {
                    cpDebug(LOG_DEBUG, "matching command found" );
                    return pendingCommand;
                }
                cpDebug(LOG_DEBUG, "found command, but Via's don't match" );
            }
        }

        ++iter;
    }

    // no match found, return 0
    cpLog( LOG_ERR, "no matching command" );
    return 0;
}


const Sptr < SipCallLeg >
SipEvent::getSipCallLeg() const
{
    assert( mySipCallLeg != 0 );

    return ( mySipCallLeg );
}


void
SipEvent::removeCall( )
{
    assert( myCallInfo != 0 );
    assert( myCallContainer != 0 );
    assert( mySipCallLeg != 0 );

    myCallContainer->removeCall(mySipCallLeg);
}


void 
SipEvent::setCallLeg()
{
    assert(mySipMsg != 0);
    mySipCallLeg = new SipCallLeg(mySipMsg->computeCallLeg());
    cpDebug(LOG_DEBUG, "set SipCallLeg, From user: %s, To user: %s",
          mySipCallLeg->getFrom().getUser().logData(),
	  mySipCallLeg->getTo().getUser().logData());
}
