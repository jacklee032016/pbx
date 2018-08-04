/*
 * $Id: SipCallLeg.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */

#include "global.h"
#include "symbols.hxx"
#include "SipUrl.hxx"
#include "SipCallLeg.hxx"

using namespace Vocal;

SipCallLeg::SipCallLeg()
    :
    from(),
    to(),
    callId(),
    cseq()
{
}


SipCallLeg::SipCallLeg( const SipFrom& newFrom, const SipTo& newTo, const SipCallId& newCallId)
    :
    callId(newCallId), cseq()
{
    setTo(newTo);
    setFrom(newFrom);
}


SipCallLeg::SipCallLeg( const Data& srcData )
{
    assert(0);
}


SipCallLeg::SipCallLeg( const SipCallLeg& src)
        :
        from(src.from),
        to(src.to),
	callId(src.callId),
        cseq(src.cseq)
{
}


SipCallLeg&
SipCallLeg::operator=( const SipCallLeg& rhs )
{
    if (&rhs != this)
    {
        from = rhs.from;
        to = rhs.to;
        callId = rhs.callId;
        cseq = rhs.cseq;
    }
    return (*this);
}


SipFrom 
SipCallLeg::getFrom() const
{
    return SipFrom(from);
}


void 
SipCallLeg::setFrom( const SipFrom& newfrom )
{
    Sptr <SipUrl> sipUrl;
    sipUrl.dynamicCast(newfrom.getUrl());
    if (sipUrl != 0)
    {
        from = sipUrl->getNameAddr();
    }
}


bool
SipCallLeg::operator == (const SipCallLeg& src) const
{
    cpDebug(LOG_DEBUG, "in SipCallLeg::operator == ");
    if ( getCallId() == src.getCallId() && cseq == src.cseq)
    {
        if ( to == src.to && from == src.from )
        {
            cpDebug(LOG_DEBUG_STACK, "Exact Match" );
            return true;
        }
        else
        {
            if ( to == src.from && from == src.to )
            {
                cpDebug(LOG_DEBUG_STACK, "To-From cross match" );
                return true;
            }
	    else{
		cpDebug(LOG_DEBUG_STACK, "NOT matched !!!!");
	    }
        }
    }
    return false;
}

bool
SipCallLeg::operator != (const SipCallLeg& src) const
{
    cpDebug(LOG_DEBUG, "in SipCallLeg::operator != ");
    return (!operator==(src));
}


bool
SipCallLeg::operator < (const SipCallLeg& rhs) const
{
    Data lhsTo = to;
    Data lhsFrom = from;
    if ( callId < rhs.callId )
    {
        cpDebug(LOG_DEBUG, "CallId < rhs.CallId" );
        return true;
    }
    else if ( callId == rhs.callId )
    {
        if(cseq < rhs.cseq)
        {
            cpDebug(LOG_DEBUG, "cseq < rhs.cseq" );
            return true;
        }
        else
        {
            cpDebug(LOG_DEBUG, "Call ID  && cseq match" );

            Data rhsTo = rhs.to;
            Data rhsFrom = rhs.from;

        /*
               cpDebug(LOG_DEBUG, "    To  : %s", to.logData() );
               cpDebug(LOG_DEBUG, "    From: %s", from.logData() );
               cpDebug(LOG_DEBUG, "rhs To  : %s", rhsTo.logData() );
               cpDebug(LOG_DEBUG, "rhs From: %s", rhsFrom.logData() );
        */

            if ( lhsFrom < lhsTo )
            {
                // Sort this
                Data temp( lhsTo );
                lhsTo = lhsFrom;
                lhsFrom = temp;
            }
            if ( rhsFrom < rhsTo )
            {
                // Sort rhs
                Data temp( rhsTo );
                rhsTo = rhsFrom;
                rhsFrom = temp;
            }

            if ( lhsTo < rhsTo )
            {
                return true;
            }
            else if ( lhsTo == rhsTo )
            {
                if ( lhsFrom < rhsFrom )
                {
                    return true;
                }
                else if ( lhsFrom == rhsFrom )
                {
                    return false;
                }
            }
        }
    }
    return false;
}


SipTo 
SipCallLeg::getTo() const
{
    return SipTo(to);
}


void 
SipCallLeg::setTo( const SipTo& newto )
{
    Sptr <SipUrl> sipUrl;
    sipUrl.dynamicCast(newto.getUrl());
    if (sipUrl != 0)
    {
        to = sipUrl->getNameAddr();
    }
}


const SipCallId& 
SipCallLeg::getCallId() const
{
    return callId;
}


void 
SipCallLeg::setCallId( const SipCallId& newcallId )
{
    callId = newcallId;
}

const SipCSeq& 
SipCallLeg::getCSeq() const
{
    return cseq;
}


void 
SipCallLeg::setCSeq( const SipCSeq& newcseq )
{
    cseq = newcseq;
}

Data 
SipCallLeg::encode() const
{
////// check if no one "important" is using this then just return from and to
////// instead of encoding the urls... and hence remove them altogether from
////// class structure!
    Data sipCallLeg;
    sipCallLeg = from;
    sipCallLeg += to;
    sipCallLeg += callId.encode();
    return sipCallLeg;
}


SipHeader*
SipCallLeg::duplicate() const
{
    return new SipCallLeg(*this);
}


bool
SipCallLeg::compareSipHeader(SipHeader* msg) const
{
    SipCallLeg* otherMsg = dynamic_cast<SipCallLeg*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}
