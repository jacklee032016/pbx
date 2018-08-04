#ifndef OpInCallFarEndAnswered_HXX
#define OpInCallFarEndAnswered_HXX
/*
 * $Id: OpInCallFarEndAnswered.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>

namespace Vocal
{


class OpInCallFarEndAnswered : public UaOperator
{
    public:
        ///
        OpInCallFarEndAnswered();
        ///
        virtual ~OpInCallFarEndAnswered();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
