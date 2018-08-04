#ifndef OpInCallBusy_HXX
#define OpInCallBusy_HXX
/*
 * $Id: OpInCallBusy.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>

namespace Vocal
{


class OpInCallBusy : public UaOperator
{
    public:
        ///
        OpInCallBusy();
        ///
        virtual ~OpInCallBusy();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
