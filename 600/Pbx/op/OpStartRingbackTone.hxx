#ifndef OpStartRingbackTone_HXX
#define OpStartRingbackTone_HXX
/*
 * $Id: OpStartRingbackTone.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include <State.hxx>
#include <SipProxyEvent.hxx>

#include "UaOperator.hxx"

namespace Vocal
{

///
class OpStartRingbackTone : public UaOperator
{
    public:
        ///
        OpStartRingbackTone();
        ///
        virtual ~OpStartRingbackTone();
        ///
        virtual const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};

 
}

#endif
