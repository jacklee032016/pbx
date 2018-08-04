#ifndef StateReRinging_HXX
#define StateReRinging_HXX
/*
 * $Id: StateReRinging.hxx,v 1.2 2007/03/02 04:19:29 lizhijie Exp $
 */

#include "State.hxx"


namespace Vocal
{

class StateReRinging : public State
{
    public:
        ///
        StateReRinging();
        ///
        virtual ~StateReRinging();
        ///
        const char* const name() const;
};
 
}

#endif
