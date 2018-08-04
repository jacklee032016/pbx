#ifndef StateConferencing_HXX
#define StateConferencing_HXX
/*
 * $Id: StateConferencing.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "State.hxx"


namespace Vocal
{

class StateConferencing : public State
{
    public:
        ///
        StateConferencing();
        ///
        virtual ~StateConferencing();
        ///
        const char* const name() const;
};
 
}

#endif
