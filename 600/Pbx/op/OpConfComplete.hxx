#ifndef OpConfComplete_HXX
#define OpConfComplete_HXX
/*
 * $Id: OpConfComplete.hxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>

namespace Vocal
{


class OpConfComplete : public UaOperator
{
    public:
        ///
        OpConfComplete();
        ///
        virtual ~OpConfComplete();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
