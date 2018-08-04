#ifndef OpStartConsultXferring_HXX
#define OpStartConsultXferring_HXX

/*
 * $Id: OpStartConsultXferring.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>


namespace Vocal
{

class OpStartConsultXferring : public UaOperator
{
    public:
        ///
        OpStartConsultXferring();
        ///
        virtual ~OpStartConsultXferring();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
