#ifndef OpBlindXferred_HXX
#define OpBlindXferred_HXX
/*
 * $Id: OpBlindXferred.hxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>

namespace Vocal
{


class OpBlindXferred : public UaOperator
{
    public:
        ///
        OpBlindXferred();
        ///
        virtual ~OpBlindXferred();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
