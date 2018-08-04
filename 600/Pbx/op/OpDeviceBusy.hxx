#ifndef OpDeviceBusy_HXX
#define OpDeviceBusy_HXX
/*
 * $Id: OpDeviceBusy.hxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>

namespace Vocal
{

class OpDeviceBusy : public UaOperator
{
    public:
        ///
        OpDeviceBusy();
        ///
        virtual ~OpDeviceBusy();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
