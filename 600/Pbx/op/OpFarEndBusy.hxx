#ifndef OpFarEndBusy_HXX
#define OpFarEndBusy_HXX
/*
 * $Id: OpFarEndBusy.hxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>

namespace Vocal
{


class OpFarEndBusy : public UaOperator
{
    public:
        ///
        OpFarEndBusy();
        ///
        virtual ~OpFarEndBusy();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
