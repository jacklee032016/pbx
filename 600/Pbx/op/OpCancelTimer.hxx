#ifndef OpCancelTimer_HXX
#define OpCancelTimer_HXX
/*
 * $Id: OpCancelTimer.hxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include <SipProxyEvent.hxx>
#include "UaOperator.hxx"

namespace Vocal
{


class OpCancelTimer : public UaOperator
{
    public:
        ///
        OpCancelTimer();
        ///
        virtual ~OpCancelTimer();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
