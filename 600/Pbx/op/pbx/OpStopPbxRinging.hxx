#ifndef OpStopPbxRinging_HXX
#define OpStopPbxRinging_HXX
/*
 * $Id: OpStopPbxRinging.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <State.hxx>
#include <SipProxyEvent.hxx>


/**
 *
 */
namespace Vocal
{

///
class OpStopPbxRinging : public UaOperator
{
    public:
        ///
        OpStopPbxRinging();
        ///
        virtual ~OpStopPbxRinging();
        ///
        virtual const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
