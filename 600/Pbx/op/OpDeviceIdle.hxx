#ifndef OpDeviceIdle_HXX
#define OpDeviceIdle_HXX
/*
 * $Id: OpDeviceIdle.hxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>

namespace Vocal
{

class OpDeviceIdle : public UaOperator
{
    public:
        ///
        OpDeviceIdle();
        ///
        virtual ~OpDeviceIdle();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
