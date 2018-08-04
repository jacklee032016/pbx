#ifndef OpInCallInvite_HXX
#define OpInCallInvite_HXX

/*
 * $Id: OpInCallInvite.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>

namespace Vocal
{


class OpInCallInvite : public UaOperator
{
    public:
        ///
        OpInCallInvite();
        ///
        virtual ~OpInCallInvite();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
