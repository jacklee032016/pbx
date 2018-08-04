/*
 * $Id: TCPServerProtocol.cxx,v 1.2 2007/03/01 20:04:33 lizhijie Exp $
 */


#include "global.h"
#include "TCPServerProtocol.hxx"
#include "TCPServerSocket.hxx"


using Vocal::Transport::TCPServerProtocol;
using Vocal::Transport::Protocol;
using Vocal::Transport::TCPServerSocket;


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
throw ( Vocal::SystemException, Vocal::SystemStatus )
{
    Sptr<TCPClientSocket>   tcpClientSocket = getTCPServerSocket().accept();
    onAcceptedConnection(tcpClientSocket);
}
