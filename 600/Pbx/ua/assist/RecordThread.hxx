/*
* $Id: RecordThread.hxx,v 1.2 2007/03/21 18:12:39 lizhijie Exp $
*/

#ifndef		__RECORDTHREAD_HXX
#define		__RECORDTHREAD_HXX

#include "ThreadIf.hxx"

namespace Vocal
{
class RecordThread : public ThreadIf
{
public:
	RecordThread();
	~RecordThread();

	virtual void shoutdown();
	
protected:
	virtual void thread();

};

}

#endif
