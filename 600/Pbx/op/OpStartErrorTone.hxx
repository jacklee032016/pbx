#ifndef OpStartErrorTone_HXX
#define OpStartErrorTone_HXX

/*
 * $Id: OpStartErrorTone.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <State.hxx>
#include <SipProxyEvent.hxx>


namespace Vocal
{

///
class OpStartErrorTone : public UaOperator
{
    public:
        ///
        OpStartErrorTone();
        ///
        virtual ~OpStartErrorTone();
        ///
        virtual const char* const  name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
