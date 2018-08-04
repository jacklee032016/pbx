#ifndef OpStopBusyTone_HXX
#define OpStopBusyTone_HXX
/*
 * $Id: OpStopBusyTone.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <State.hxx>
#include <SipProxyEvent.hxx>


namespace Vocal
{

///
class OpStopBusyTone : public UaOperator
{
    public:
        ///
        OpStopBusyTone();
        ///
        virtual ~OpStopBusyTone();
        ///
        virtual const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
