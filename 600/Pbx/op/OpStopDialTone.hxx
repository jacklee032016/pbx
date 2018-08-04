#ifndef OpStopDialTone_HXX
#define OpStopDialTone_HXX
/*
 * $Id: OpStopDialTone.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <State.hxx>
#include <SipProxyEvent.hxx>


namespace Vocal
{

///
class OpStopDialTone : public UaOperator
{
    public:
        ///
        OpStopDialTone();
        ///
        virtual ~OpStopDialTone();
        ///
        virtual const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
