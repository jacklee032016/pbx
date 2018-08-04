#ifndef StateFastBusy_HXX
#define StateFastBusy_HXX

/*
 * $Id: StateFastBusy.hxx,v 1.2 2007/03/02 04:19:29 lizhijie Exp $
 */

#include "State.hxx"


namespace Vocal
{

class StateFastBusy : public State
{
    public:
        ///
        StateFastBusy();
        ///
        virtual ~StateFastBusy();
        ///
        const char* const name() const;
};
 
}

#endif
