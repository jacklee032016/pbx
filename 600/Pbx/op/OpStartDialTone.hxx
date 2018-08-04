#ifndef OpStartDialTone_HXX
#define OpStartDialTone_HXX

/*
 * $Id: OpStartDialTone.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <State.hxx>
#include <SipProxyEvent.hxx>


namespace Vocal
{

/**
 * Starts dialtone when the ua receives an offhook event
 * @author jeffgao
 * @see OpStopDialTone
 */
class OpStartDialTone : public UaOperator
{
    public:
        ///
        OpStartDialTone();
        ///
        virtual ~OpStartDialTone();
        ///
        virtual const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};

#endif



 
}


