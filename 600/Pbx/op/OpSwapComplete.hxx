#ifndef OpSwapComplete_HXX
#define OpSwapComplete_HXX
/*
 * $Id: OpSwapComplete.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>


namespace Vocal
{

class OpSwapComplete : public UaOperator
{
    public:
        ///
        OpSwapComplete();
        ///
        virtual ~OpSwapComplete();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
