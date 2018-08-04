/*
 * $Id: SipDebuggingInterface.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */

#include "global.h"
#include "TCPServerSocket.hxx"
#include "ThreadIf.hxx"
#include "IPAddress.hxx"
#include "SipDebuggingInterface.hxx"
#include "Lock.hxx"
#include "Mutex.hxx"

using namespace Vocal;


SipClientDebuggingInterface::SipClientDebuggingInterface(Sptr<TCPClientSocket> sock, SipDebuggingInterface& parent)
    : mySock(sock),
      myParent(parent)
{
}

void
SipClientDebuggingInterface::thread()
{
    char buffer[256];
    int pos = 0;
    while(!isShutdown())
    {
	int count = 0;
	char tmp;
	try
	{
	    count = mySock->receive(&tmp, 1);
	}
	catch (...)
	{
	    shutdown();
	    mySock = 0;
	    return;
	}
	if(count) 
	{
	    if(tmp == '\r' || tmp == '\n' || (pos > 250))
	    {
		// run command
		buffer[pos] = '\0';
		if(strcmp(buffer, "status") == 0)
		{
		    Vocal::Threads::Lock x(myParent.myMutex);
		    for(SipDebuggingInterface::StatusList::iterator i 
			    = myParent.myStatus.begin() ;
			i != myParent.myStatus.end() ; ++i)
		    {
			Data x = (*i)->status();
                        LocalScopeAllocator lo;
			mySock->send(x.getData(lo));
			mySock->send("\n");
		    }
		}
		if(strcmp(buffer, "quit") == 0)
		{
		    shutdown();
		    mySock = 0;
		    return;
		}
		pos = 0;
	    }
	    else if(tmp == '\x08') 
	    {
		if(pos > 0)
		{
		    pos--;
		}
	    }
	    else
	    {
		buffer[pos++] = tmp;
	    }
	}
    }
}



SipParentDebuggingInterface::SipParentDebuggingInterface(int port, 
							 SipDebuggingInterface& parent)
    : ThreadIf(),
      mySock(IPAddress(port),1),
      myParent(parent)
{
}


void
SipParentDebuggingInterface::thread()
{
    while(!isShutdown())
    {
	// lame, but simple -- max 1 connection at a time
	Sptr<TCPClientSocket> s = mySock.accept();
	if(myInterface != 0)
	{
	    if(myInterface->isShutdown())
	    {
		myInterface->join();
		myInterface = 0;
	    }
	}

	if((s != 0) && (myInterface == 0))
	{
	    myInterface = new SipClientDebuggingInterface(s, myParent);
	    myInterface->run();
	}
	else if(s != 0)
	{
	    s->send("sorry, you cannot connect\r\n");
	    s = 0;
	}
    }
    if(myInterface != 0)
    {
	myInterface->shutdown();
	myInterface->join();
	myInterface = 0;
    }
}


SipParentDebuggingInterface::~SipParentDebuggingInterface()
{
    if(myInterface != 0)
    {
	myInterface->shutdown();
	myInterface->join();
	myInterface = 0;
    }
}


SipDebuggingInterface::SipDebuggingInterface(int port)
    :
    myInterface(port, *this)
{
    myInterface.run();
}


void
SipDebuggingInterface::add(Sptr<SipDebuggingStatus> item)
{
    Vocal::Threads::Lock x(myMutex);
    myStatus.push_back(item);
}


SipDebuggingInterface::~SipDebuggingInterface()
{
    myInterface.shutdown();
    myInterface.join();
}


SipDebuggingStatus::SipDebuggingStatus()
{
}


SipDebuggingStatus::~SipDebuggingStatus()
{
}
