#ifndef OpCwRing_HXX
#define OpCwRing_HXX
/*
 * $Id: OpCwRing.hxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>

namespace Vocal
{


class OpCwRing : public UaOperator
{
    public:
        ///
        OpCwRing();
        ///
        virtual ~OpCwRing();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
