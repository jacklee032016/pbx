#ifndef StateTrying_HXX
#define StateTrying_HXX
/*
 * $Id: StateTrying.hxx,v 1.2 2007/03/02 04:19:29 lizhijie Exp $
 */

#include <cassert>
#include "State.hxx"


namespace Vocal
{

class StateTrying : public State
{
    public:
        ///
        StateTrying();
        ///
        virtual ~StateTrying();
        ///
        const char* const name() const;
};
 
}

#endif
