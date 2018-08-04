#ifndef OpSwapCalls_HXX
#define OpSwapCalls_HXX
/*
 * $Id: OpSwapCalls.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>
#include "UaDeviceEvent.hxx"

namespace Vocal
{

class OpSwapCalls : public UaOperator
{
    public:
        ///
        OpSwapCalls();
        ///
        virtual ~OpSwapCalls();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );

    private:

        bool putContactOnhold( const Sptr < UaDeviceEvent > deviceEvent );
        void restoreHeldCall( const Sptr < UaDeviceEvent > deviceEvent );
        bool firstCall;
};
 
}

#endif
