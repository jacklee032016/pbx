#ifndef StateConsultXferring_HXX
#define StateConsultXferring_HXX

/*
 * $Id: StateConsultXferring.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "State.hxx"


namespace Vocal
{

class StateConsultXferring : public State
{
    public:
        ///
        StateConsultXferring();
        ///
        virtual ~StateConsultXferring();
        ///
        const char* const name() const;
};
 
}

#endif
