/*
* $Id: VThreadPoolConnectTest.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/

#include "VThreadPool.cxx"
#include "Condition.hxx"
#include "cpLog.h"
#include "Tcp_ServerSocket.hxx"
#include <map>
#include "VMutex.h"
#include "Lock.hxx"

using Vocal::Threads::Lock;
VMutex myMutex;


class ConnContainer
{
    public:
	Connection conn;
	bool active;

	ConnContainer()
	    : active(false)
	{
	}
	ConnContainer(const Connection& c)
	    : conn(c),
	      active(false)
	{
	}

	ConnContainer(const ConnContainer& c)
	    : conn(c.conn),
	      active(c.active)
	{
	}
	const ConnContainer& operator=(const ConnContainer& c)
	{
	    if(&c != this)
	    {
		conn = c.conn;
		active = c.active;
	    }
	    return *this;
	}
};

map <int, ConnContainer> myMap;

void doWork(void* msg)
{
    ConnContainer* conn;
    conn = reinterpret_cast<ConnContainer*>(msg);
    assert(conn);
    cpLog(LOG_ERR, "got connection");

    char buf[256];
    int num;
    conn->conn.readLine(buf, 256, num);
    vusleep(100);
    int fd = conn->conn.getConnId();
    conn->conn.close();
    Lock myLock(myMutex);
    myMap.erase(fd);
}


int main()
{
    VThreadPool pool(10);
    TcpServerSocket server(5000);

    for(;;)
    {
	if(server.getServerConn().isReadReady())
	{
	    ConnContainer conn;
	    server.accept(conn.conn);
	    myMutex.lock();
	    myMap[conn.conn.getConnId()] = conn;
	    myMutex.unlock();
	}

	myMutex.lock();
	for(map <int, ConnContainer>::iterator i = myMap.begin(); 
	    i != myMap.end() ; 
	    ++i)
	{
	    ConnContainer* ptr = &((*i).second);
	    if(ptr->conn.isLive() && ptr->conn.isReadReady())
	    {
		VFunctor func(doWork, ptr);
		pool.addFunctor(func);
	    }
	}
	myMutex.unlock();

    }
}
