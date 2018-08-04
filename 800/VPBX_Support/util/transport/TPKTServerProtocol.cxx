/*
* $Id: TPKTServerProtocol.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include "TPKTServerProtocol.hxx"
#include "TPKTServerSocket.hxx"


using Assist::Transport::TPKTServerProtocol;
using Assist::Transport::Protocol;
using Assist::Transport::TPKTServerSocket;


TPKTServerProtocol::TPKTServerProtocol(const char * name)
    :	Protocol(name)
{
    awaitIncoming(true);
}


TPKTServerProtocol::~TPKTServerProtocol()
    throw ( Assist::SystemException )
{
}


void	    	    	    
TPKTServerProtocol::onIncomingAvailable()
throw ( Assist::SystemException, Assist::SystemStatus )
{
    Sptr<TPKTClientSocket>   tpktClientSocket = getTPKTServerSocket().accept();
    onAcceptedConnection(tpktClientSocket);
}
