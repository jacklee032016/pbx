#ifndef OpStartBlindXferring_HXX
#define OpStartBlindXferring_HXX

/*
 * $Id: OpStartBlindXferring.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>


namespace Vocal
{

///
class OpStartBlindXferring : public UaOperator
{
    public:
        ///
        OpStartBlindXferring();
        ///
        virtual ~OpStartBlindXferring();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
