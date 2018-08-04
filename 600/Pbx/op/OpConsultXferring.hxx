#ifndef OpConsultXferring_HXX
#define OpConsultXferring_HXX
/*
 * $Id: OpConsultXferring.hxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>


namespace Vocal
{


class OpConsultXferring : public UaOperator
{
    public:
        ///
        OpConsultXferring();
        ///
        virtual ~OpConsultXferring();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
        {}
};
 
}

#endif
