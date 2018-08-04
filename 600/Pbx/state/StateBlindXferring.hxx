#ifndef StateBlindXferring_HXX
#define StateBlindXferring_HXX
/*
 * $Id: StateBlindXferring.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "State.hxx"


namespace Vocal
{

class StateBlindXferring : public State
{
    public:
        ///
        StateBlindXferring();
        ///
        virtual ~StateBlindXferring();
        ///
        const char* const name() const;
};
 
}

#endif
