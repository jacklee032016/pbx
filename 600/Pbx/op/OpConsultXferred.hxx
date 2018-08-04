#ifndef OpConsultXferred_HXX
#define OpConsultXferred_HXX
/*
 * $Id: OpConsultXferred.hxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>

namespace Vocal
{


class OpConsultXferred : public UaOperator
{
    public:
        ///
        OpConsultXferred();
        ///
        virtual ~OpConsultXferred();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
