#ifndef OpAnswerCall_HXX
#define OpAnswerCall_HXX

/*
 * $Id: OpAnswerCall.hxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>

namespace Vocal
{

class OpAnswerCall : public UaOperator
{
    public:
        ///
        OpAnswerCall();
        ///
        virtual ~OpAnswerCall();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
