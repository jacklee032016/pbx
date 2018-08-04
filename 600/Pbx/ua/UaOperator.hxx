#ifndef UaOperator_HXX
#define UaOperator_HXX

/*
 * $Id: UaOperator.hxx,v 1.2 2007/03/02 04:19:29 lizhijie Exp $
 */

#include <Operator.hxx>
#include <SipProxyEvent.hxx>

namespace Vocal
{

class UaOperator : public Operator
{
    public:
        ///
        UaOperator();
        ///
        virtual ~UaOperator();
        ///
        const char* const name() const;
    
    protected:
        ///
        bool setTimer( const Sptr < SipProxyEvent > event, unsigned int timeout );
        ///
        bool cancelTimer( const Sptr < SipProxyEvent > event );
        ///
        bool isTimeout( const Sptr < SipProxyEvent > evnet );
};
 
}

#endif
