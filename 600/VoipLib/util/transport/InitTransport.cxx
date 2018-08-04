/*
 * $Id: InitTransport.cxx,v 1.2 2007/03/01 20:04:33 lizhijie Exp $
 */


#include "global.h"
#include "Mutex.hxx"

using Vocal::Threads::Mutex;

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
