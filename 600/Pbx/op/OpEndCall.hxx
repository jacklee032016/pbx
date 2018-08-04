#ifndef OpEndCall_HXX
#define OpEndCall_HXX
/*
 * $Id: OpEndCall.hxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>

namespace Vocal
{


class OpEndCall : public UaOperator
{
    public:
        ///
        OpEndCall();
        ///
        virtual ~OpEndCall();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
