/*
 * $Id: SignalAction.cxx,v 1.2 2007/03/01 20:04:33 lizhijie Exp $
 */

#include "global.h"
#include "SignalAction.hxx"


using Vocal::Signals::SignalAction;
using Vocal::Signals::SignalSet;


SignalAction::SignalAction(const SignalSet & signalSet, const char * pName)
    :   mySignalSet(signalSet),
        mySignalCaught(false),
        myName(pName ? pName : "SignalAction")
{
}


SignalAction::~SignalAction()
{
}


const SignalSet &
SignalAction::signalSet() const
{
    return ( mySignalSet );
}


void	
SignalAction::onSignal(int signum, int error, int code)
{
    mySignalCaught = true;
    handleSignal(signum, error, code);
}



const string &
SignalAction::name() const
{
    return ( myName );
}


ostream &
SignalAction::writeTo(ostream & out) const
{
    return ( out << myName << ", signalSet = { " << mySignalSet << " }" );
}


bool
SignalAction::signalCaught() const
{
    return ( mySignalCaught );
}


void
SignalAction::reset()
{
    mySignalCaught = false;
}


void	
SignalAction::handleSignal(int, int, int)
{
}
