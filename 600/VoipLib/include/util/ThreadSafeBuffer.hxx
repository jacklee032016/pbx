#ifndef THREADSAFEBUFFER_HXX_
#define THREADSAFEBUFFER_HXX_
/*
 * $Id: ThreadSafeBuffer.hxx,v 1.2 2007/03/01 20:04:29 lizhijie Exp $
 */

#include "Lock.hxx"
#include "VMutex.h"


class ThreadSafeBuffer
{
    public:
	ThreadSafeBuffer();
	~ThreadSafeBuffer();
	const char* c_str(const char* buf, int length);
	const char* get_c_str() const;
    private:
	VMutex myMutex;
        char* myData;
};


inline
ThreadSafeBuffer::ThreadSafeBuffer()
    : myMutex(),
      myData(0)
{
    
}


inline
ThreadSafeBuffer::~ThreadSafeBuffer()
{
    if(myData)
    {
	delete[] myData;
    }
}


inline
const char* 
ThreadSafeBuffer::c_str(const char* buf, int length)
{
    Vocal::Threads::Lock x(myMutex);
    if(myData)
    {
	delete[] myData;
    }
    myData = new char[length + 1];
    memcpy(myData, buf, length);
    myData[length] = '\0';
    return myData;
}


inline
const char* 
ThreadSafeBuffer::get_c_str() const
{
    return myData;
}

#endif
