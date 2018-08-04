#ifndef __TASK_EVENT_HXX__
#define __TASK_EVENT_HXX__
/*
 * $Id: TaskEvent.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "PbxEvent.hxx"

namespace Assist
{

typedef  void *(*_task_start_routing)(void *);

class TaskEvent : public PbxEvent
{
	public:
		TaskEvent(_task_start_routing routing, void *data);

		TaskEvent();
		virtual ~TaskEvent();

		// The name of the extending class.
		virtual const char* const name() const 
		{
			return "TASK-MSG";
		};

		void run();
		
		void setMyRouting(_task_start_routing _routing);
			
	protected:

	private:
		TaskEvent(const TaskEvent &);
		const TaskEvent & operator=(const TaskEvent &);

		_task_start_routing		myRouting;
};
 
}

#endif

