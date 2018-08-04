#ifndef OpHoldTargetOk_HXX
#define OpHoldTargetOk_HXX
/*
 * $Id: OpHoldTargetOk.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>

namespace Vocal
{


class OpHoldTargetOk : public UaOperator
{
    public:
        ///
        OpHoldTargetOk();
        ///
        virtual ~OpHoldTargetOk();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
