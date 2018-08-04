#ifndef OpBlindXferring_HXX
#define OpBlindXferring_HXX
/*
 * $Id: OpBlindXferring.hxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>

namespace Vocal
{


class OpBlindXferring : public UaOperator
{
    public:
        ///
        OpBlindXferring();
        ///
        virtual ~OpBlindXferring();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
        {}
};
 
}

#endif
