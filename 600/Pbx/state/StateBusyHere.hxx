#ifndef StateBusyHere_HXX
#define StateBusyHere_HXX

/*
 * $Id: StateBusyHere.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "State.hxx"


namespace Vocal
{

class StateBusyHere : public State
{
    public:
        ///
        StateBusyHere();
        ///
        virtual ~StateBusyHere();
        ///
        const char* const name() const;
};
 
}

#endif
