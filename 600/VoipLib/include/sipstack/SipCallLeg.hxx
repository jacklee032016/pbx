#ifndef SipCallLeg_HXX
#define SipCallLeg_HXX
/*
 * $Id: SipCallLeg.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include "global.h"

#include "Data.hxx"

#include "SipFrom.hxx"
#include "SipCallId.hxx"
#include "SipCSeq.hxx"
#include "SipTo.hxx"
#include "SipHeader.hxx"

namespace Vocal
{

/// data container for CallLeg header
class SipCallLeg : public SipHeader
{
    public:
        /// Create one with default values
        SipCallLeg();
        virtual ~SipCallLeg()
            {}

        SipCallLeg( const SipFrom& from, const SipTo& to, 
                    const SipCallId& callId);
        /*** Create by decoding the data string passed in. This is the decode
             or parse. This call should only be used inside the stack and is
             not part of the API */
        SipCallLeg( const Data& srcData );
        SipCallLeg( const SipCallLeg& src);
        SipCallLeg& operator=( const SipCallLeg& rhs );

        virtual bool operator == (const SipCallLeg& src) const;
        virtual bool operator != (const SipCallLeg& src) const;

        virtual bool operator < (const SipCallLeg& src) const;

        SipFrom getFrom() const;
        void setFrom(const SipFrom& newfrom);

        SipTo getTo() const;
        void setTo(const SipTo& newto);

        const SipCallId& getCallId() const;
        void setCallId(const SipCallId& newcallId);

        const SipCSeq& getCSeq() const;
        void setCSeq(const SipCSeq& newcseq);

        /*** return the encoded string version of this. This call should only be
             used inside the stack and is not part of the API */
        Data encode() const;

	/// method for copying sip headers of any type without knowing which type
	SipHeader* duplicate() const;
	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;

    private:
        Data from;
        Data to;
        SipCallId callId;
        SipCSeq cseq;
};

} // namespace Vocal


inline bool
operator<( const Sptr<Vocal::SipCallLeg>& left, const Sptr<Vocal::SipCallLeg>&  right)
{
    return (*left < *right);
}

#endif
