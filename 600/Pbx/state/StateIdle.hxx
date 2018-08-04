#ifndef StateIdle_HXX
#define StateIdle_HXX
/*
 * $Id: StateIdle.hxx,v 1.2 2007/03/02 04:19:29 lizhijie Exp $
 */

#include "State.hxx"


namespace Vocal
{

class StateIdle : public State
{
    public:
        ///
        StateIdle();
        ///
        virtual ~StateIdle();
        ///
        const char* const name() const;
};
 
}

#endif
