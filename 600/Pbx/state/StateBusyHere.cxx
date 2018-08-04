/*
 * $Id: StateBusyHere.cxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "global.h"
#include "StateBusyHere.hxx"

#include "OpBusyHere.hxx"
#include "OpBusyAck.hxx"
#include "OpStartTimer.hxx"
#include "OpCancelTimer.hxx"
#include "OpTimeFinish.hxx"

using namespace Vocal;


StateBusyHere::StateBusyHere()
{
     //added by lijie 2005-06-03
   // addEntryOperator( new OpStartTimer );
	
    addOperator( new OpBusyHere );
    addOperator( new OpBusyAck );

    //added by lijie 2005-06-03
    addOperator( new OpTimeFinish );
		 
    //added by lijie	2005-06-03
    addExitOperator( new OpCancelTimer );
}


StateBusyHere::~StateBusyHere()
{
}


const char* const
StateBusyHere::name() const
{
    return "StateBusyHere";
}
