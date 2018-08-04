#ifndef StateInCallTrying_HXX
#define StateInCallTrying_HXX

/*
 * $Id: StateInCallTrying.hxx,v 1.2 2007/03/02 04:19:29 lizhijie Exp $
 */

#include "State.hxx"


namespace Vocal
{

class StateInCallTrying : public State
{
    public:
        ///
        StateInCallTrying();
        ///
        virtual ~StateInCallTrying();
        ///
        const char* const name() const;
};
 
}

#endif
