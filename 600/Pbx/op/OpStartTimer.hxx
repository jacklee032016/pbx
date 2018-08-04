#ifndef OpStartTimer_HXX
#define OpStartTimer_HXX

/*
 * $Id: OpStartTimer.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include <SipProxyEvent.hxx>
#include "UaOperator.hxx"

namespace Vocal
{

class OpStartTimer : public UaOperator
{
    public:
        ///
        OpStartTimer();
        ///
        virtual ~OpStartTimer();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
