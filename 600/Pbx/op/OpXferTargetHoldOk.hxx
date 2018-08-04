#ifndef OpXferTargetHoldOk_HXX
#define OpXferTargetHoldOk_HXX

/*
 * $Id: OpXferTargetHoldOk.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>


namespace Vocal
{

class OpXferTargetHoldOk : public UaOperator
{
    public:
        ///
        OpXferTargetHoldOk();
        ///
        virtual ~OpXferTargetHoldOk();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
