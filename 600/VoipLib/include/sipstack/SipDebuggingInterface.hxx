#ifndef SIP_DEBUGGING_INTERFACE_HXX_
#define SIP_DEBUGGING_INTERFACE_HXX_
/*
 * $Id: SipDebuggingInterface.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
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

namespace Vocal
{

using Vocal::Transport::TCPServerSocket;
using Vocal::Transport::TCPClientSocket;
using Vocal::Transport::IPAddress;
using Vocal::Threads::Lock;

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
        Vocal::Threads::Mutex myMutex;
};
 
} // namespace Vocal

#endif
