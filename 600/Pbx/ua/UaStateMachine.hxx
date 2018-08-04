#ifndef UaStateMachine_Hxx
#define UaStateMachine_Hxx

/*
 * $Id: UaStateMachine.hxx,v 1.2 2007/03/02 04:19:29 lizhijie Exp $
 */

#include <Feature.hxx>


namespace Vocal
{

/**
 *
 */
class UaStateMachine : public Feature
{
    public:
        ///
        UaStateMachine();

        ///
        ~UaStateMachine();

        ///
        const char* const name() const;
        ///
        void process( const Sptr < SipProxyEvent > event );
        ///
        Sptr < State > findState( const string stateName );

};
 
}

#endif
