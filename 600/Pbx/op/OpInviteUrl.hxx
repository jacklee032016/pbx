#ifndef OpInviteUrl_HXX
#define OpInviteUrl_HXX
/*
 * $Id: OpInviteUrl.hxx,v 1.3 2007/03/07 18:24:08 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>

namespace Vocal
{

class OpInviteUrl : public UaOperator
{
    public:
        OpInviteUrl();
        virtual ~OpInviteUrl();

        const char* const name() const;

        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
