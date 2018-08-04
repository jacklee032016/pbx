/*
 * $Id: TPKTClientProtocol.cxx,v 1.2 2007/03/01 20:04:33 lizhijie Exp $
 */


#include "global.h"
#include "TPKTClientProtocol.hxx"
#include "TPKTClientSocket.hxx"
#include "SystemStatus.hxx"
#include "VLog.hxx"
#include <cerrno>


using Vocal::Transport::TPKTClientProtocol;
using Vocal::Transport::Protocol;
using Vocal::Transport::TPKTClientSocket;
using Vocal::Logging::VLog;


TPKTClientProtocol::TPKTClientProtocol(const char * name)
    :	Protocol(name ? name : "TPKTClientProtocol"),
    	pkt_(0),
    	pktPosition_(0),
	pktLength_(0)
{
    awaitIncoming(true);
}


TPKTClientProtocol::~TPKTClientProtocol()
{
}


void	    	    	    
TPKTClientProtocol::onIncomingAvailable()
throw ( Vocal::SystemException, 
    	Vocal::SystemStatus, 
	Vocal::Transport::ConnectionBrokenException )
{
    const string    fn("TPKTClientProtocol::onIncomingAvailable");
    VLog    	    log(fn);
    
    // If we are at the start of a new packet, read the header.
    // If we don't receive the whole header, a status of EAGAIN will be thrown
    // and it will be the user's responsibility to call it again later,
    // most likely after selecting on the file descriptor again.
    //
    if ( pkt_ == 0 )
    {
    	pktLength_ = getTPKTClientSocket().receiveTPKTHeader();
        pkt_ = new vector<u_int8_t>(pktLength_);
	pktPosition_ = 0;
    }

    // Get the current position of the array and start adding there.
    // If we don't receive the whole packet, throw a status of EAGAIN
    // and it will be the user's responsibility to call it again later,
    // most likely after selecting  on the file descriptor again.
    //
    u_int8_t	*   bytes   	= &((*pkt_)[pktPosition_]);
    int     	    bytesLeft 	= pktLength_ - pktPosition_;

    pktPosition_ += getTPKTClientSocket().receive(bytes, bytesLeft);

    if ( pktPosition_ < pktLength_ )
    {
    	throw Vocal::SystemStatus(fn + " parital TPKT header received.",
	    	    	    	__FILE__, __LINE__, EAGAIN);
    }

    // We have a packet, so notify the user.
    //

    VDEBUG(log) << fn << ": TPKT arrived: length = " << pktLength_ 
    	    	<< VDEBUG_END(log);

    Sptr< vector<u_int8_t> >	newPkt = pkt_;

    pkt_    	    = 0;
    pktPosition_    = 0;
    pktLength_      = 0;

    // This may cause the object to be deleted. Don't put anything
    // after onTPKTArrived.
    //    		
    onTPKTArrived(*newPkt);
}
