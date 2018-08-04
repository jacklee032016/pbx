#ifndef OpRingingInvite_HXX
#define OpRingingInvite_HXX
/*
 * $Id: OpRingingInvite.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>


namespace Vocal
{


class OpRingingInvite : public UaOperator
{
    public:
        ///
        OpRingingInvite();
        ///
        virtual ~OpRingingInvite();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
