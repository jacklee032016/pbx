#ifndef OpStartRinging_HXX
#define OpStartRinging_HXX
/*
 * $Id: OpStartRinging.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <State.hxx>
#include <SipProxyEvent.hxx>


namespace Vocal
{

///
class OpStartRinging : public UaOperator
{
    public:
        ///
        OpStartRinging();
        ///
        virtual ~OpStartRinging();
        ///
        virtual const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
