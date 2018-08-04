#ifndef StateDialing_HXX
#define StateDialing_HXX

/*
 * $Id: StateDialing.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "State.hxx"


namespace Vocal
{

class StateDialing : public State
{
    public:
        ///
        StateDialing();
        ///
        virtual ~StateDialing();
        ///
        const char* const name() const;
};
 
}

#endif
