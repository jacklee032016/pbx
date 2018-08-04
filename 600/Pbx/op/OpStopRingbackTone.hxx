#ifndef OpStopRingbackTone_HXX
#define OpStopRingbackTone_HXX
/*
 * $Id: OpStopRingbackTone.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <State.hxx>
#include <SipProxyEvent.hxx>


namespace Vocal
{

///
class OpStopRingbackTone : public UaOperator
{
    public:
        ///
        OpStopRingbackTone();
        ///
        virtual ~OpStopRingbackTone();
        ///
        virtual const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
