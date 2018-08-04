/*
 * $Id: TPKTServerProtocol.cxx,v 1.2 2007/03/01 20:04:33 lizhijie Exp $
 */


#include "global.h"
#include "TPKTServerProtocol.hxx"
#include "TPKTServerSocket.hxx"


using Vocal::Transport::TPKTServerProtocol;
using Vocal::Transport::Protocol;
using Vocal::Transport::TPKTServerSocket;


TPKTServerProtocol::TPKTServerProtocol(const char * name)
    :	Protocol(name)
{
    awaitIncoming(true);
}


TPKTServerProtocol::~TPKTServerProtocol()
    throw ( Vocal::SystemException )
{
}


void	    	    	    
TPKTServerProtocol::onIncomingAvailable()
throw ( Vocal::SystemException, Vocal::SystemStatus )
{
    Sptr<TPKTClientSocket>   tpktClientSocket = getTPKTServerSocket().accept();
    onAcceptedConnection(tpktClientSocket);
}
