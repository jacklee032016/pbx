#ifndef OpDigitsResp_HXX
#define OpDigitsResp_HXX
/*
 * $Id: OpDigitsResp.hxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>


namespace Vocal
{

class OpDigitsResp : public UaOperator
{
    public:
        ///
        OpDigitsResp();
        ///
        virtual ~OpDigitsResp();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};

}

#endif
