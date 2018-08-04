/*
* $Id: RecordThread.cxx,v 1.2 2007/03/21 18:12:39 lizhijie Exp $
*/

#if	__CDR_OPEN__

#include "RecordThread.hxx"
#include "SimulateDB.hxx"
using namespace SDB;  

RecordThread::RecordThread()
{
	SimulateDB::create(); 
}

RecordThread::~RecordThread()
{
	SimulateDB::destroy();
}

void RecordThread::thread()    
{
	while (true)
	{
		SimulateDB::instance()->writePipe();
	}
	
}

void RecordThread::shoutdown()
{

}

#endif 

