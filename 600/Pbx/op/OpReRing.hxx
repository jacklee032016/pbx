#ifndef OpReRing_HXX
#define OpReRing_HXX
/*
 * $Id: OpReRing.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>


namespace Vocal
{


class OpReRing : public UaOperator
{
    public:
        ///
        OpReRing();
        ///
        virtual ~OpReRing();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
