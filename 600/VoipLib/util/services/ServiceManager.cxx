/*
 * $Id: ServiceManager.cxx,v 1.2 2007/03/01 20:04:33 lizhijie Exp $
 */


#include "global.h"
#include "ServiceManager.hxx"


using Vocal::Services::ServiceManager;
using Vocal::Signals::SignalAction;


ServiceManager::ServiceManager(SignalAction * signalAction)
    :	mySignalAction(signalAction)
{
}


ServiceManager::~ServiceManager()
{
}


