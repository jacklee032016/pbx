#ifndef OpDropTalk_HXX
#define OpDropTalk_HXX
/*
 * $Id: OpDropTalk.hxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>


namespace Vocal
{

class OpDropTalk : public UaOperator
{
    public:
        ///
        OpDropTalk();
        ///
        virtual ~OpDropTalk();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
