#ifndef StateRinging_HXX
#define StateRinging_HXX
/*
 * $Id: StateRinging.hxx,v 1.2 2007/03/02 04:19:29 lizhijie Exp $
 */

#include <cassert>
#include "State.hxx"


namespace Vocal
{

class StateRinging : public State
{
    public:
        ///
        StateRinging();
        ///
        virtual ~StateRinging();
        ///
        const char* const name() const;
};
 
}

#endif
