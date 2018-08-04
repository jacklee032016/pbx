#ifndef OpStopErrorTone_HXX
#define OpStopErrorTone_HXX

/*
 * $Id: OpStopErrorTone.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <State.hxx>
#include <SipProxyEvent.hxx>


namespace Vocal
{

///
class OpStopErrorTone : public UaOperator
{
    public:
        ///
        OpStopErrorTone();
        ///
        virtual ~OpStopErrorTone();
        ///
        virtual const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
