#ifndef StateInCall_HXX
#define StateInCall_HXX

/*
 * $Id: StateInCall.hxx,v 1.2 2007/03/02 04:19:29 lizhijie Exp $
 */

#include "State.hxx"


namespace Vocal
{

class StateInCall : public State
{
    public:
        ///
        StateInCall();
        ///
        virtual ~StateInCall();
        ///
        const char* const name() const;
};
 
}

#endif
