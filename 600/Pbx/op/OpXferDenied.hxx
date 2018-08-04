#ifndef OpXferDenied_HXX
#define OpXferDenied_HXX

/*
 * $Id: OpXferDenied.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>


namespace Vocal
{

class OpXferDenied : public UaOperator
{
    public:
        ///
        OpXferDenied();
        ///
        virtual ~OpXferDenied();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
