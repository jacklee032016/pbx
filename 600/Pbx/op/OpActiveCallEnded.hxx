#ifndef OpActiveCallEnded_HXX
#define OpActiveCallEnded_HXX
/*
 * $Id: OpActiveCallEnded.hxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>

namespace Vocal
{

class OpActiveCallEnded : public UaOperator
{
    public:
        ///
        OpActiveCallEnded();
        ///
        virtual ~OpActiveCallEnded();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
