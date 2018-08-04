#ifndef OpFwdDigit_HXX
#define OpFwdDigit_HXX
/*
 * $Id: OpFwdDigit.hxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include "SipProxyEvent.hxx"


namespace Vocal
{

///
class OpFwdDigit : public UaOperator
{
    public:
        ///
        OpFwdDigit();
        ///
        virtual ~OpFwdDigit();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
