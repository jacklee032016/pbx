#ifndef OpBusyAck_HXX
#define OpBusyAck_HXX
/*
 * $Id: OpBusyAck.hxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>

namespace Vocal
{


class OpBusyAck : public UaOperator
{
    public:
        ///
        OpBusyAck();
        ///
        virtual ~OpBusyAck();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
