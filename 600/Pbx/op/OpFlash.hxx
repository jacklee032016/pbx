#ifndef OpFlash_HXX
#define OpFlash_HXX
/*
 * $Id: OpFlash.hxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>

namespace Vocal
{


class OpFlash : public UaOperator
{
    public:
        ///
        OpFlash();
        ///
        virtual ~OpFlash();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
        {}
};
 
}

#endif
