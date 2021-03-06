#ifndef OpStopAudio_HXX
#define OpStopAudio_HXX
/*
 * $Id: OpStopAudio.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <State.hxx>
#include <SipProxyEvent.hxx>


namespace Vocal
{

///
class OpStopAudio : public UaOperator
{
    public:
        ///
        OpStopAudio();
        ///
        virtual ~OpStopAudio();
        ///
        virtual const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
