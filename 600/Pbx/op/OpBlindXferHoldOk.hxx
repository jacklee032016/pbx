#ifndef OpBlindXferHoldOk_HXX
#define OpBlindXferHoldOk_HXX
/*
 * $Id: OpBlindXferHoldOk.hxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>

namespace Vocal
{


class OpBlindXferHoldOk : public UaOperator
{
    public:
        ///
        OpBlindXferHoldOk();
        ///
        virtual ~OpBlindXferHoldOk();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
