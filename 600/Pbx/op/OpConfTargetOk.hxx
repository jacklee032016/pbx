#ifndef OpConfTargetOk_HXX
#define OpConfTargetOk_HXX
/*
 * $Id: OpConfTargetOk.hxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>

namespace Vocal
{


class OpConfTargetOk : public UaOperator
{
    public:
        ///
        OpConfTargetOk();
        ///
        virtual ~OpConfTargetOk();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
