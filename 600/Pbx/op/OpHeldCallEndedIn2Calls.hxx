#ifndef OpHeldCallEndedIn2Calls_HXX
#define OpHeldCallEndedIn2Calls_HXX
/*
 * $Id: OpHeldCallEndedIn2Calls.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>

namespace Vocal
{


class OpHeldCallEndedIn2Calls : public UaOperator
{
    public:
        ///
        OpHeldCallEndedIn2Calls();
        ///
        virtual ~OpHeldCallEndedIn2Calls();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
