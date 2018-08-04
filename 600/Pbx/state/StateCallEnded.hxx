#ifndef StateCallEnded_HXX
#define StateCallEnded_HXX
/*
 * $Id: StateCallEnded.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "State.hxx"


namespace Vocal
{

class StateCallEnded : public State
{
    public:
        ///
        StateCallEnded();
        ///
        virtual ~StateCallEnded();
        ///
        const char* const name() const;
};
 
}

#endif
