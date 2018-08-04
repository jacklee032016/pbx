#ifndef OpXfereeComplete_HXX
#define OpXfereeComplete_HXX
/*
 * $Id: OpXfereeComplete.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>


namespace Vocal
{

class OpXfereeComplete : public UaOperator
{
    public:
        ///
        OpXfereeComplete();
        ///
        virtual ~OpXfereeComplete();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
