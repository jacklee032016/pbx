#ifndef OpConfError_HXX
#define OpConfError_HXX
/*
 * $Id: OpConfError.hxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>


namespace Vocal
{


class OpConfError : public UaOperator
{
    public:
        ///
        OpConfError();
        ///
        virtual ~OpConfError();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
