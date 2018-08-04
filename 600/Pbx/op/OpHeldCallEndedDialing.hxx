#ifndef OpHeldCallEndedDialing_HXX
#define OpHeldCallEndedDialing_HXX

/*
 * $Id: OpHeldCallEndedDialing.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>

namespace Vocal
{


class OpHeldCallEndedDialing : public UaOperator
{
    public:
        ///
        OpHeldCallEndedDialing();
        ///
        virtual ~OpHeldCallEndedDialing();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
