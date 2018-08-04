#ifndef StateInCallDialing_HXX
#define StateInCallDialing_HXX
/*
 * $Id: StateInCallDialing.hxx,v 1.2 2007/03/02 04:19:29 lizhijie Exp $
 */

#include "State.hxx"

namespace Vocal
{

class StateInCallDialing : public State
{
    public:
        ///
        StateInCallDialing();
        ///
        virtual ~StateInCallDialing();
        ///
        const char* const name() const;
};
 
}

#endif
