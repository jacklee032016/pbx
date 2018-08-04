#ifndef StateBusy_HXX
#define StateBusy_HXX
/*
 * $Id: StateBusy.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "State.hxx"


namespace Vocal
{

class StateBusy : public State
{
    public:
        ///
        StateBusy();
        ///
        virtual ~StateBusy();
        ///
        const char* const name() const;
};
 
}

#endif
