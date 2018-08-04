#ifndef OpHangupError_HXX
#define OpHangupError_HXX
/*
 * $Id: OpHangupError.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>

namespace Vocal
{


class OpHangupError : public UaOperator
{
    public:
        ///
        OpHangupError();
        ///
        virtual ~OpHangupError();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
