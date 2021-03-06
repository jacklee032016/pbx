#ifndef OpStartCallWaiting_HXX
#define OpStartCallWaiting_HXX
/*
 * $Id: OpStartCallWaiting.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>


namespace Vocal
{

class OpStartCallWaiting : public UaOperator
{
    public:
        ///
        OpStartCallWaiting();
        ///
        virtual ~OpStartCallWaiting();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
