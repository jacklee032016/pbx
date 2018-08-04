#ifndef OpSuspendAudio_HXX
#define OpSuspendAudio_HXX

/*
 * $Id: OpSuspendAudio.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include "State.hxx"


/**
 *
 */
namespace Vocal
{

///
class OpSuspendAudio : public UaOperator
{
    public:
        ///
        OpSuspendAudio();
        ///
        virtual ~OpSuspendAudio();
        ///
        virtual const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
