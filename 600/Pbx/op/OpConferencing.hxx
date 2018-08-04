#ifndef OpConferencing_HXX
#define OpConferencing_HXX
/*
 * $Id: OpConferencing.hxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>
#include <SipEvent.hxx>
#include <InviteMsg.hxx>

#include "UaCallInfo.hxx"

namespace Vocal
{


class OpConferencing : public UaOperator
{
    public:
        ///
        OpConferencing();
        ///
        virtual ~OpConferencing();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
    private:
        ///
        void sendTransfer( Sptr < InviteMsg > msg,
			   Sptr< SipUrl > transferToUrl,
			   Sptr < UaCallInfo > call,
			   const Sptr < SipEvent > sipEvent );
};
 
}

#endif
