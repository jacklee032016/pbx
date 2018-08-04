#ifndef OpReInvite_HXX
#define OpReInvite_HXX
/*
 * $Id: OpReInvite.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>


namespace Vocal
{


class OpReInvite : public UaOperator
{
    public:
        ///
        OpReInvite();
        ///
        virtual ~OpReInvite();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
