/*
* $Id: TPKTClientSocket.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include "TPKTClientSocket.hxx"
#include "IPAddress.hxx"
#include "TransportCommon.hxx"
#include "SystemException.hxx"
#include "SystemStatus.hxx"
#include "VLog.hxx"
#include <cerrno>


using Assist::Transport::TPKTClientSocket;
using Assist::Transport::TCPClientSocket;
using Assist::Transport::IPAddress;
using Assist::IO::file_descriptor_t;
using Assist::Logging::VLog;


const u_int8_t      TPKTClientSocket::TPKT_FLAG_[2] = { 0x03, 0x00 };


TPKTClientSocket::TPKTClientSocket(
    const char      	*   name
)
throw ( Assist::SystemException )
    :	TCPClientSocket(name ? name : "TPKTClient"),
    	headerPosition_(0)
{
    header_[0] = header_[1] = header_[2] = header_[3] = 0;
}


TPKTClientSocket::TPKTClientSocket(
    const IPAddress 	&   localAddr,
    const char      	*   name
)
throw ( Assist::SystemException )
    : 	TCPClientSocket(localAddr, (name ? name : "TPKTClient")),
    	headerPosition_(0)
{
    header_[0] = header_[1] = header_[2] = header_[3] = 0;
}


TPKTClientSocket::TPKTClientSocket(
    const IPAddress 	&   localAddr,
    const IPAddress 	&   remoteAddr,
    const char      	*   name
)
throw ( Assist::SystemException, Assist::SystemStatus )
    : 	TCPClientSocket(localAddr, remoteAddr, (name ? name : "TPKTClient")),
    	headerPosition_(0)
{
    header_[0] = header_[1] = header_[2] = header_[3] = 0;
}


TPKTClientSocket::TPKTClientSocket(
    file_descriptor_t 	    fd,
    const IPAddress 	&   remoteAddr,
    const char      	*   name
)
throw ( Assist::SystemException, Assist::SystemStatus )
    : 	TCPClientSocket(fd, remoteAddr, (name ? name : "TPKTClient")),
    	headerPosition_(0)
{
    header_[0] = header_[1] = header_[2] = header_[3] = 0;
}


TPKTClientSocket::~TPKTClientSocket()
{
}


int 	
TPKTClientSocket::send(const string & message)
throw ( Assist::SystemException, 
    	Assist::SystemStatus, 
	Assist::Transport::ConnectionBrokenException )
{
    sendTPKTHeader(message.size());
    
    return ( StreamClientSocket::send(message) );
}


int 	
TPKTClientSocket::send(const char * message)
throw ( Assist::SystemException, 
    	Assist::SystemStatus, 
	Assist::Transport::ConnectionBrokenException )
{
    int messageSize = ( message ? strlen(message) : 0 );
    
    sendTPKTHeader(messageSize);
    
    return ( StreamClientSocket::send(message) );
}


int 	
TPKTClientSocket::send(const vector<u_int8_t> & message)
throw ( Assist::SystemException, 
    	Assist::SystemStatus, 
	Assist::Transport::ConnectionBrokenException )
{
    sendTPKTHeader(message.size());
    
    return ( StreamClientSocket::send(message) );
}


int 	
TPKTClientSocket::send(const u_int8_t * message, size_t messageSize)
throw ( Assist::SystemException, 
    	Assist::SystemStatus, 
	Assist::Transport::ConnectionBrokenException )
{
    sendTPKTHeader(messageSize);
    
    return ( StreamClientSocket::send(message, messageSize) );
}
				

u_int16_t 	
TPKTClientSocket::receiveTPKTHeader()
throw ( Assist::SystemException, 
    	Assist::SystemStatus, 
	Assist::Transport::ConnectionBrokenException )
{
    const string    fn("TPKTClientSocket::receiveTPKTHeader");
    VLog    	    log(fn);

    u_int8_t	*   bytes   	= &header_[headerPosition_];
    int     	    bytesLeft 	= 4 - headerPosition_;
    
    headerPosition_ += StreamClientSocket::receive(bytes, bytesLeft);

    if ( headerPosition_ < 4 )
    {
    	throw Assist::SystemStatus(fn + " parital TPKT header received.",
	    	    	    	__FILE__, __LINE__, EAGAIN);
    }

    if ( header_[0] != TPKT_FLAG_[0] || header_[1] != TPKT_FLAG_[1] )
    {
    	VDEBUG(log) << fn << ": TPKT header/reserved version incorrect."
	    	   << VDEBUG_END(log);
				
    }
    
    u_int16_t	    length = ntohs(*(u_int16_t *)(&header_[2]));

    length = (length < 4 ? 0 : length - 4);

    clearTPKTHeader();
    
    VDEBUG(log) << fn << ": remaining length = " << length << VDEBUG_END(log);
    
    return ( length );
}


void	
TPKTClientSocket::clearTPKTHeader()
{
    header_[0] = header_[1] = header_[2] = header_[3] = 0;
    headerPosition_ = 0;
}
		

void	    
TPKTClientSocket::sendTPKTHeader(u_int16_t  size)
throw ( Assist::SystemException, 
    	Assist::SystemStatus, 
	Assist::Transport::ConnectionBrokenException )
{
    const string    fn("TPKTClientSocket::sendTPKTHeader");
    VLog    	    log(fn);

    // Include the TPKT header in the size.
    //
    size += 4;
    
    VDEBUG(log) << "TPKT size: " << size << VDEBUG_END(log);
    
    u_int8_t	    header[4];
    u_int16_t	*   length = (u_int16_t *)&header[2];
    
    header[0] = TPKT_FLAG_[0];
    header[1] = TPKT_FLAG_[1];
    *length = htons(size);
    
    StreamClientSocket::send(header, 4);
}
