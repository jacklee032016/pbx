#ifndef OpHeldCallEndedTrying_HXX
#define OpHeldCallEndedTrying_HXX
/*
 * $Id: OpHeldCallEndedTrying.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>

namespace Vocal
{


class OpHeldCallEndedTrying : public UaOperator
{
    public:
        ///
        OpHeldCallEndedTrying();
        ///
        ///
        virtual ~OpHeldCallEndedTrying();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
