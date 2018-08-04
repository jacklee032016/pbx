#ifndef OpTimeFinish_HXX
#define OpTimeFinish_HXX
/*
 * $Id: OpTimeFinish.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include <SipProxyEvent.hxx>
#include "UaOperator.hxx"

namespace Vocal
{

class OpTimeFinish : public UaOperator
{
    public:
        ///
        OpTimeFinish();
        ///
        virtual ~OpTimeFinish();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
