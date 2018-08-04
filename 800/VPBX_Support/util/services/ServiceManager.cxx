/*
* $Id: ServiceManager.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include "ServiceManager.hxx"


using Assist::Services::ServiceManager;
using Assist::Signals::SignalAction;


ServiceManager::ServiceManager(SignalAction * signalAction)
    :	mySignalAction(signalAction)
{
}


ServiceManager::~ServiceManager()
{
}


