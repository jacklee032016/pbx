#ifndef OpFarEndAnswered_HXX
#define OpFarEndAnswered_HXX
/*
 * $Id: OpFarEndAnswered.hxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>

namespace Vocal
{


class OpFarEndAnswered : public UaOperator
{
    public:
        ///
        OpFarEndAnswered();
        ///
        virtual ~OpFarEndAnswered();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
