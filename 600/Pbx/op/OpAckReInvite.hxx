#ifndef OpAckReInvite_HXX
#define OpAckReInvite_HXX
/*
 * $Id: OpAckReInvite.hxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include "SipProxyEvent.hxx"


namespace Vocal
{

class OpAckReInvite : public UaOperator
{
    public:
        ///
        OpAckReInvite();
        ///
        virtual ~OpAckReInvite();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}


#endif
