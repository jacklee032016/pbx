#ifndef StateBeingXferred_HXX
#define StateBeingXferred_HXX
/*
 * $Id: StateBeingXferred.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "State.hxx"


namespace Vocal
{

class StateBeingXferred : public State
{
    public:
        ///
        StateBeingXferred();
        ///
        virtual ~StateBeingXferred();
        ///
        const char* const name() const;
};
 
}

#endif
