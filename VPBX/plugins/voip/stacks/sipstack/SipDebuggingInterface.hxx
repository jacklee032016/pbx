#ifndef SIP_DEBUGGING_INTERFACE_HXX_
#define SIP_DEBUGGING_INTERFACE_HXX_

/*
* $Log: SipDebuggingInterface.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipDebuggingInterface.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include <list>

#include "global.h"
#include "TransportCommon.hxx"
#include "TCPServerSocket.hxx"
#include "ThreadIf.hxx"
#include "IPAddress.hxx"
#include "TCPClientSocket.hxx"
#include "SipTransactionDB.hxx"
#include "SipUdpConnection.hxx"
#include "Mutex.hxx"
#include "Lock.hxx"

namespace Assist
{

using Assist::Transport::TCPServerSocket;
using Assist::Transport::TCPClientSocket;
using Assist::Transport::IPAddress;
using Assist::Threads::Lock;

class SipDebuggingInterface;

class SipClientDebuggingInterface : public ThreadIf
{
    public:
	SipClientDebuggingInterface(Sptr<TCPClientSocket> sock, 
				    SipDebuggingInterface& parent);
	void thread();
	
    private:
	Sptr<TCPClientSocket> mySock;
	SipDebuggingInterface& myParent;
};


class SipParentDebuggingInterface : public ThreadIf
{
    public:
    SipParentDebuggingInterface(int port, SipDebuggingInterface& parent);
    ~SipParentDebuggingInterface();

    void thread();
    private:
	TCPServerSocket mySock;
	Sptr<SipClientDebuggingInterface> myInterface;
	SipDebuggingInterface& myParent;
};


class SipDebuggingStatus
{
    public:
	SipDebuggingStatus();
        virtual ~SipDebuggingStatus();
        virtual Data status() = 0;
};


class SipDebuggingInterface
{
    public:
	SipDebuggingInterface(int port); 
	~SipDebuggingInterface();
	void add(Sptr<SipDebuggingStatus> item);

    public:
        typedef list < Sptr<SipDebuggingStatus> > StatusList;
        SipParentDebuggingInterface myInterface;
	StatusList myStatus;
        Assist::Threads::Mutex myMutex;
};
 
} 

#endif
