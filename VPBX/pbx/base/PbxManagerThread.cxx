/*
 * $Id: PbxManagerThread.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "global.h"
#include "cpLog.h"
#include <cassert>

#include "PbxManager.hxx"
#include "PbxManagerThread.hxx"

using namespace Assist;

PbxManagerThread::PbxManagerThread( const Sptr <PbxManager> manager )
{
	myManager = manager;
//	memset(myName, 0, VPBX_NAME_LENGTH );
//	sprintf(myName, "%s", _name);
}

PbxManagerThread::~PbxManagerThread()
{
}

const char *PbxManagerThread::getName() const
{
	return myManager->getName() ;
};

void PbxManagerThread::thread()
{
	cpLogSetLabelThread (VThread::selfId(), getName() );

  	cpLog(LOG_DEBUG, "PID of %s is %d" ,getName(), getpid() );
	
	while ( true )
	{
		myManager->mainLoop( 0 );

  //	cpLog(LOG_DEBUG, "PID of %s is %d" ,getName(), getpid() );
		if ( isShutdown() == true)
			return;
	}
}

