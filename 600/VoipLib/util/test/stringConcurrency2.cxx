/*
* $Id: stringConcurrency2.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/

#include <string>
#include "VThread.hxx"
#include "VMutex.h"
#include "LockHelper.hxx"
#include <stdio.h>
#include "Fifo.h"

using namespace Vocal;

inline string fn(int i)
{
    switch(i)
    {
    case 1:
	return "one ";
	break;
    case 2:
	return "two ";
	break;
    case 3:
	return "three ";
	break;
    default:
	return "zero ";
    }
}


void* thread1(void*)
{
    for(;;)
    {
	// allocate a string, then use it as a c_str
	cout << fn(rand() % 4).c_str();
    }
    return 0;
}


struct List
{
    List* next;
    string x;
};


Vocal::Fifo <List*> f;


void* thread2(void*)
{
    for(;;)
    {
	// allocate a string, then use it as a c_str
#if 0
	if (f.size() > 100)
	{
	    usleep(100);
	}
#endif
	List* tmp = new List;
	tmp->x = fn(rand() % 4);
	f.add(tmp);
    }
    return 0;
}


void* thread3(void*)
{
    for(;;)
    {
	List* next = f.getNext();
	delete next;
    }
}

int main()
{
    //    setupProfilingStuff();

    string junk;

    VThread my_thread1;
    VThread my_thread2;
    VThread my_thread3;
    VThread my_thread4;

    my_thread1.spawn(thread1, 0);
    my_thread2.spawn(thread1, 0);
    my_thread3.spawn(thread2, 0);
    my_thread4.spawn(thread3, 0);

    for (int i = 0; i < 1000000000; i++);

    my_thread1.join();
    my_thread2.join();
    my_thread3.join();
    my_thread4.join();

    //    DumpAddressMap();
    //    finish();
    return 0;
}
