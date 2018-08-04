#ifndef OpStartBusyTone_HXX
#define OpStartBusyTone_HXX
/*
 * $Id: OpStartBusyTone.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <State.hxx>
#include <SipProxyEvent.hxx>


namespace Vocal
{

///
class OpStartBusyTone : public UaOperator
{
    public:
        ///
        OpStartBusyTone();
        ///
        virtual ~OpStartBusyTone();
        ///
        virtual const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
