#ifndef RSVP_HANDLER_HXX_
#define RSVP_HANDLER_HXX_

#include "SipSdp.hxx"

using Assist::SipSdp;

void setupRsvp(SipSdp& localSdp, SipSdp& remoteSdp);

void rsvpAckHandler(Sptr<SipSdp> localSdp, Sptr<SipSdp> remoteSdp);

void rsvpFarEndAnswered(Sptr<SipSdp> localSdp, Sptr<SipSdp> remoteSdp);

#endif

