/*
* $Id: ManagedObject.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "ManagedObject.hxx"

#include "PbxManager.hxx"
#include "PbxTimerEvent.hxx"

using namespace Assist;

/*
ManagedObject::ManagedObject( )
{
}
*/

ManagedObject::ManagedObject(Sptr <PbxManager> _parent)
	:myParent(_parent),
	myTimer(0)
{
}

ManagedObject::~ManagedObject(void)
{
	cpLog(LOG_DEBUG, "Destruncting.....");
}

#if 0
void ManagedObject::setTimer( const Sptr <PbxTimerEvent> timerEvent )
{
	assert ( timerEvent != 0 );
	myTimer = timerEvent;
}
Sptr <PbxTimerEvent> ManagedObject::getTimer() const
{
	return myTimer;
}
void ManagedObject::removeTimer()
{
	myTimer = 0;
}
#endif

Sptr <PbxManager>  ManagedObject::getParent()
{
	return myParent;
}

