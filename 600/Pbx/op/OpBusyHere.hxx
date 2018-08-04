#ifndef OpBusyHere_HXX
#define OpBusyHere_HXX
/*
 * $Id: OpBusyHere.hxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>

namespace Vocal
{


class OpBusyHere : public UaOperator
{
    public:
        ///
        OpBusyHere();
        ///
        virtual ~OpBusyHere();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
