#include "RsvpHandler.hxx"
#include "PbxConfiguration.hxx"
#include "NetworkAddress.h"
#include "SipSdp.hxx"

#ifdef HAS_RSVPLIB
#include "SuaRsvp.hxx"
#endif

using Assist::SipSdp;

void setupRsvp(SipSdp& localSdp, SipSdp& remoteSdp)
{
#ifdef HAS_RSVPLIB
    // RSVP stuff ported from old OpRing.cxx
    if ( PbxConfiguration::instance()->getRsvpOn() )
    {
	SuaRsvp& suaRsvpSender = UaDevice::instance()->getSuaRsvpSender();
	NetworkAddress remoteAddress;
	NetworkAddress myAddress;

#if 0
	if ( remoteSdp.isValidSdp() )
	{
#endif
	    remoteAddress.setHostName( remoteSdp.getConnAddress().getData() );
	    remoteAddress.setPort( remoteSdp.getRtpPort() );
	    suaRsvpSender.session( &remoteAddress);
#if 0
	}
	else
	{
	    cpLog( LOG_ERR, "Invalid Remote Sdp");
	}
#endif

#if 0
	if ( localSdp.isValidSdp() )
	{
#endif
	    myAddress.setHostName( localSdp.getConnAddress().getData() );
	    myAddress.setPort( localSdp.getRtpPort() );
	    suaRsvpSender.sender( &myAddress);
#if 0
	}
	else
	{
	    cpLog( LOG_ERR, "Invalid local Sdp");
	}
#endif
    }
#endif
}


void rsvpAckHandler(Sptr<SipSdp> localSdp, Sptr<SipSdp> remoteSdp)
{
    assert( localSdp != 0 );
    assert( remoteSdp != 0 );

#ifdef HAS_RSVPLIB
    // RSVP stuff ported from sua OpAck.cxx
    if ( PbxConfiguration::instance()->getRsvpOn() )
    {
        //send RSVP message
        SuaRsvp& suaRsvpReserv = UaDevice::instance()->getSuaRsvpReserv();

        NetworkAddress myAddress;
        NetworkAddress remoteAddress;

//        if ( remoteSdp->isValidSdp() )
        {
            remoteAddress.setHostName( remoteSdp->getConnAddress().getData() );
            remoteAddress.setPort( remoteSdp->getRtpPort() );
            remoteAddress.getSockAddr(suaRsvpReserv.src_addr);
        }

#if 0
        else
        {
            cpLog( LOG_ERR, "Invalid Remote Sdp");
        }
#endif

//        if ( localSdp->isValidSdp() )
        {
            myAddress.setHostName( localSdp->getConnAddress().getData() );
            myAddress.setPort( localSdp->getRtpPort() );
            suaRsvpReserv.session(&myAddress);
            myAddress.setPort(0);
            suaRsvpReserv.reserve(&myAddress);
        }

#if 0
        else
        {
            cpLog( LOG_ERR, "Invalid local Sdp");
        }
#endif
    }
#endif


}


void rsvpFarEndAnswered(Sptr<SipSdp> localSdp, Sptr<SipSdp> remoteSdp)
{
    assert(localSdp != 0);
    assert(remoteSdp != 0);

#ifdef HAS_RSVPLIB
    if (PbxConfiguration::instance()->getRsvpOn() )
    {
        //send RSVP message
        SuaRsvp &suaRsvpSender = UaDevice::instance()->getSuaRsvpSender();
        SuaRsvp &suaRsvpReserv = UaDevice::instance()->getSuaRsvpReserv();

        NetworkAddress myAddress;
        NetworkAddress remoteAddress;

        myAddress.setHostName( localSdp->getConnAddress().getData() );
        myAddress.setPort( localSdp->getRtpPort() );

        remoteAddress.setHostName( remoteSdp->getConnAddress().getData() );
        remoteAddress.setPort( remoteSdp->getRtpPort() );

        remoteAddress.getSockAddr(suaRsvpReserv.src_addr);

        suaRsvpReserv.session(&myAddress);
        myAddress.setPort(0);
        suaRsvpReserv.reserve(&myAddress);

        suaRsvpSender.session(&remoteAddress);

        myAddress.setPort( localSdp->getRtpPort() );
        suaRsvpSender.sender(&myAddress);
    }
    // end RSVP stuff
#endif
}
