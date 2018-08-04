#ifndef OpTimeOver_HXX
#define OpTimeOver_HXX
/*
 * $Id: OpTimeOver.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include <SipProxyEvent.hxx>
#include "UaOperator.hxx"

namespace Vocal
{

class OpTimeOver : public UaOperator
{
    public:
        ///
        OpTimeOver();
        ///
        virtual ~OpTimeOver();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
