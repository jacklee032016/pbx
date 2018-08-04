#ifndef OpInCallStopTrying_HXX
#define OpInCallStopTrying_HXX
/*
 * $Id: OpInCallStopTrying.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */


#include "UaOperator.hxx"
#include <State.hxx>
#include <SipProxyEvent.hxx>


namespace Vocal
{


/**
 *
 */
class OpInCallStopTrying : public UaOperator
{
    public:
        ///
        OpInCallStopTrying();
        ///
        virtual ~OpInCallStopTrying();
        ///
        virtual const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
