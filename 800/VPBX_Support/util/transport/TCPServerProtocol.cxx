/*
* $Id: TCPServerProtocol.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include "TCPServerProtocol.hxx"
#include "TCPServerSocket.hxx"


using Assist::Transport::TCPServerProtocol;
using Assist::Transport::Protocol;
using Assist::Transport::TCPServerSocket;


TCPServerProtocol::TCPServerProtocol(const char * name)
    :	Protocol(name)
{
    awaitIncoming(true);
}


TCPServerProtocol::~TCPServerProtocol()
{
}


void	    	    	    
TCPServerProtocol::onIncomingAvailable()
throw ( Assist::SystemException, Assist::SystemStatus )
{
    Sptr<TCPClientSocket>   tcpClientSocket = getTCPServerSocket().accept();
    onAcceptedConnection(tcpClientSocket);
}
