#ifndef OpTerminateTrying_HXX
#define OpTerminateTrying_HXX
/*
 * $Id: OpTerminateTrying.hxx,v 1.3 2007/03/07 18:24:08 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <State.hxx>
#include <SipProxyEvent.hxx>

namespace Vocal
{

class OpTerminateTrying : public UaOperator
{
    public:
        OpTerminateTrying();
        virtual ~OpTerminateTrying();
        virtual const char* const name() const;
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
