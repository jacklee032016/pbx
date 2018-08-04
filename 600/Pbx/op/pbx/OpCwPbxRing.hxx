#ifndef OpCwPbxRing_HXX
#define OpCwPbxRing_HXX

/*
 * $Id: OpCwPbxRing.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>

namespace Vocal
{


class OpCwPbxRing : public UaOperator
{
    public:
        ///
        OpCwPbxRing();
        ///
        virtual ~OpCwPbxRing();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
