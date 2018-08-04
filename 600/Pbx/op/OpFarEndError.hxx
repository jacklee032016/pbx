#ifndef OpFarEndError_HXX
#define OpFarEndError_HXX
/*
 * $Id: OpFarEndError.hxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>

namespace Vocal
{


class OpFarEndError : public UaOperator
{
    public:
        ///
        OpFarEndError();
        ///
        virtual ~OpFarEndError();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
