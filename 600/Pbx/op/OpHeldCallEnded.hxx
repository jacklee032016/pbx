#ifndef OpHeldCallEnded_HXX
#define OpHeldCallEnded_HXX
/*
 * $Id: OpHeldCallEnded.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>

namespace Vocal
{


class OpHeldCallEnded : public UaOperator
{
    public:
        ///
        OpHeldCallEnded();
        ///
        virtual ~OpHeldCallEnded();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
        {}
};
 
}

#endif
