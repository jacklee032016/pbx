#ifndef OpAck_HXX
#define OpAck_HXX
/*
 * $Id: OpAck.hxx,v 1.3 2007/03/03 18:39:27 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include "SipProxyEvent.hxx"

namespace Vocal
{

// Ack operator -- triggered when we receive an ACK
class OpAck : public UaOperator
{
    public:
        ///
        OpAck();
        ///
        virtual ~OpAck();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
        ///

    private:
};
 
}

#endif

