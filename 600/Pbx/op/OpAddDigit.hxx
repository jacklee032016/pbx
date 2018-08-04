#ifndef OpAddDigit_HXX
#define OpAddDigit_HXX
/*
 * $Id: OpAddDigit.hxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>

namespace Vocal
{

class OpAddDigit : public UaOperator
{
    public:
        ///
        OpAddDigit();
        ///
        virtual ~OpAddDigit();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
