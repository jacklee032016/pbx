#ifndef OpStartAudioDuplex_HXX
#define OpStartAudioDuplex_HXX
/*
 * $Id: OpStartAudioDuplex.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <State.hxx>
#include <SipProxyEvent.hxx>


namespace Vocal
{

///
class OpStartAudioDuplex : public UaOperator
{
    public:
        ///
        OpStartAudioDuplex();
        ///
        virtual ~OpStartAudioDuplex();
        ///
        virtual const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
