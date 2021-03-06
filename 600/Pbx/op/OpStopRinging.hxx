#ifndef OpStopRinging_HXX
#define OpStopRinging_HXX

/*
 * $Id: OpStopRinging.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <State.hxx>
#include <SipProxyEvent.hxx>


/**
 *
 */
namespace Vocal
{

///
class OpStopRinging : public UaOperator
{
    public:
        ///
        OpStopRinging();
        ///
        virtual ~OpStopRinging();
        ///
        virtual const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
