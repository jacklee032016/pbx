#ifndef StateIn2Calls_HXX
#define StateIn2Calls_HXX

/*
 * $Id: StateIn2Calls.hxx,v 1.2 2007/03/02 04:19:29 lizhijie Exp $
 */

#include "State.hxx"


namespace Vocal
{

class StateIn2Calls : public State
{
    public:
        ///
        StateIn2Calls();
        ///
        virtual ~StateIn2Calls();
        ///
        const char* const name() const;
};
 
}

#endif
