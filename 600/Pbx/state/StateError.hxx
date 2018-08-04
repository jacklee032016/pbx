#ifndef StateError_HXX
#define StateError_HXX

/*
 * $Id: StateError.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "State.hxx"

namespace Vocal
{

class StateError : public State
{
    public:
        ///
        StateError();
        ///
        virtual ~StateError();
        ///
        const char* const name() const;
};
 
}

#endif
