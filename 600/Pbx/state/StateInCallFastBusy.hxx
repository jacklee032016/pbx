#ifndef StateInCallFastBusy_HXX
#define StateInCallFastBusy_HXX
/*
 * $Id: StateInCallFastBusy.hxx,v 1.2 2007/03/02 04:19:29 lizhijie Exp $
 */

#include "State.hxx"


namespace Vocal
{

class StateInCallFastBusy : public State
{
    public:
        ///
        StateInCallFastBusy();
        ///
        virtual ~StateInCallFastBusy();
        ///
        const char* const name() const;
};
 
}

#endif
