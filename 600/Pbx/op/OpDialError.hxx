#ifndef OpDialError_HXX
#define OpDialError_HXX
/*
 * $Id: OpDialError.hxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>

namespace Vocal
{


class OpDialError : public UaOperator
{
    public:
        ///
        OpDialError();
        ///
        virtual ~OpDialError();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
