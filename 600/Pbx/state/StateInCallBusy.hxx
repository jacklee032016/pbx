#ifndef StateInCallBusy_HXX
#define StateInCallBusy_HXX
/*
 * $Id: StateInCallBusy.hxx,v 1.2 2007/03/02 04:19:29 lizhijie Exp $
 */

#include "State.hxx"


namespace Vocal
{

class StateInCallBusy : public State
{
    public:
        ///
        StateInCallBusy();
        ///
        virtual ~StateInCallBusy();
        ///
        const char* const name() const;
};
 
}

#endif
