#ifndef OpStopFastBusyTone_HXX
#define OpStopFastBusyTone_HXX

/*
 * $Id: OpStopFastBusyTone.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <State.hxx>
#include <SipProxyEvent.hxx>


namespace Vocal
{

///
class OpStopFastBusyTone : public UaOperator
{
    public:
        ///
        OpStopFastBusyTone();
        ///
        virtual ~OpStopFastBusyTone();
        ///
        virtual const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
