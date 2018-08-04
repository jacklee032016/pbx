#ifndef OpTerminateCall_HXX
#define OpTerminateCall_HXX
/*
 * $Id: OpTerminateCall.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>


namespace Vocal
{

class OpTerminateCall : public UaOperator
{
    public:
        ///
        OpTerminateCall();
        ///
        virtual ~OpTerminateCall();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
