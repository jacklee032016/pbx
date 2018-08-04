#ifndef OpStartCall_HXX
#define OpStartCall_HXX
/*
 * $Id: OpStartCall.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>


namespace Vocal
{

class OpStartCall : public UaOperator
{
    public:
        ///
        OpStartCall();
        ///
        virtual ~OpStartCall();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
