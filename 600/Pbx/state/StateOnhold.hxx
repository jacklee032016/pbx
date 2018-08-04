#ifndef StateOnhold_HXX
#define StateOnhold_HXX
/*
 * $Id: StateOnhold.hxx,v 1.2 2007/03/02 04:19:29 lizhijie Exp $
 */

#include "State.hxx"


namespace Vocal
{

class StateOnhold : public State
{
    public:
        ///
        StateOnhold();
        ///
        virtual ~StateOnhold();
        ///
        const char* const name() const;
};
 
}

#endif
