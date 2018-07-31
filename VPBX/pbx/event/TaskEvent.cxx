/*
 * $Id: TaskEvent.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "global.h"
#include "TaskEvent.hxx"
#include "ThreadIf.hxx"
#include "PbxAgent.hxx"

using namespace Assist;

TaskEvent::TaskEvent()
	: PbxEvent( )
{
}

TaskEvent::TaskEvent(_task_start_routing routing, void *data)
	: PbxEvent( ),
	myRouting(routing)
{
	setPrivate(data);
//	Sptr <TaskEvent> task = NULL;
//	task.dynamicCast(mySelf);
//	assert(task!= 0);
	PbxAgent::instance()->getGateway()->addTask(this);
}


TaskEvent::~TaskEvent()
{
	cpDebug(LOG_DEBUG, "Task deconstructing.....");
}

void TaskEvent::run()
{
	pthread_t 			pthid;
	
	if(myRouting != NULL)
		pthread_create(&pthid, NULL, myRouting, getPrivate() );

	return;
}


