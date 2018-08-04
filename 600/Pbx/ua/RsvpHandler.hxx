#ifndef RSVP_HANDLER_HXX_
#define RSVP_HANDLER_HXX_
/*
 * $Id: RsvpHandler.hxx,v 1.2 2007/03/02 04:19:29 lizhijie Exp $
 */

#include "SipSdp.hxx"

using Vocal::SipSdp;

void setupRsvp(SipSdp& localSdp, SipSdp& remoteSdp);

void rsvpAckHandler(Sptr<SipSdp> localSdp, Sptr<SipSdp> remoteSdp);

void rsvpFarEndAnswered(Sptr<SipSdp> localSdp, Sptr<SipSdp> remoteSdp);

#endif
