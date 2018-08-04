/*
 * $Id: StateIdle.cxx,v 1.3 2007/03/07 18:24:09 lizhijie Exp $
 */

#include "global.h"
#include "StateIdle.hxx"

#include "OpStartCall.hxx"
#include "OpRing.hxx"
#include "OpOnHook.hxx"
#include "UaConfiguration.hxx"
#include "OpAuthentication.hxx"
#include "OpAuthorization.hxx"

using namespace Vocal;

StateIdle::StateIdle()
{
	addOperator( new OpStartCall );
	/* added by wuaiwu, 2005/07/07 */
	//cpDebug(LOG_DEBUG, "getAuthentication = %s", UaConfiguration::instance()->getAuthentication().c_str());
	 if (UaConfiguration::instance()->getAuthentication() != "Off")
	{
	    addOperator( new OpAuthentication );   
	    addOperator( new OpAuthorization );                     
	}  
	 /* end of added */
	addOperator( new OpRing );
	addOperator( new OpOnHook );  // bizarre case
}

StateIdle::~StateIdle()
{
}

const char* const StateIdle::name() const
{
	return "StateIdle";
}

