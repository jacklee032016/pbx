#ifndef OpReInviteAuthenticated_HXX
#define OpReInviteAuthenticated_HXX
/*
 * $Id: OpReInviteAuthenticated.hxx,v 1.3 2007/03/03 18:39:28 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include "SipProxyEvent.hxx"

namespace Vocal
{


class OpReInviteAuthenticated : public UaOperator
{
    public:
        ///
        OpReInviteAuthenticated();
        ///
        virtual ~OpReInviteAuthenticated();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};

}

#endif

