#ifndef OpTimeout_HXX
#define OpTimeout_HXX
/*
 * $Id: OpTimeout.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include <SipProxyEvent.hxx>
#include "UaOperator.hxx"

namespace Vocal
{

class OpTimeout : public UaOperator
{
    public:
        ///
        OpTimeout();
        ///
        virtual ~OpTimeout();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
