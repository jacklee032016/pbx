/*
* $Id: InitTransport.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include "Mutex.hxx"

using Assist::Threads::Mutex;

int initTransport()
{
	int err = 0;

#ifdef WIN32
	static Mutex lock;
	static bool hasInitialized = false;

	lock.lock();

	if(!hasInitialized)
	{
		hasInitialized = true;
		WORD wVersionRequested;
		WSADATA wsaData;
 
		wVersionRequested = MAKEWORD( 2, 2 );
 
		err = WSAStartup( wVersionRequested, &wsaData );
	}

	lock.unlock();
#endif
	return err;
}
