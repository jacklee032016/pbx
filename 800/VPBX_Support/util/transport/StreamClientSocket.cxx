/*
* $Id: StreamClientSocket.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include "TransportCommon.hxx"
#include "StreamClientSocket.hxx"
#include "SystemException.hxx"
#include "SystemStatus.hxx"
#include "ConnectionBrokenException.hxx"
#include "VLog.hxx"
#include <cerrno>


using Assist::Transport::StreamClientSocket;
using Assist::Transport::Socket;
using Assist::Transport::TransportAddress;
using Assist::Transport::AddressFamily;
using Assist::IO::file_descriptor_t;
using Assist::Logging::VLog;
using Assist::SystemException;
using Assist::SystemStatus;


StreamClientSocket::StreamClientSocket(
    const AddressFamily     & 	addressFamily,
    const char	    	    *	name
)
throw ( Assist::SystemException )
    :   Socket( addressFamily, 
    	    	SocketType(SOCK_STREAM), 
		(name ? name : "StreamClient")),
    	remoteAddr_(0),
	localAddrUpdated_(false),
	connectionCompleted_(false)
{
}


StreamClientSocket::StreamClientSocket(
    const TransportAddress  & 	localAddr,
    const char      	    *	name
)
throw ( Assist::SystemException )
    :	Socket( localAddr, 
    	    	SocketType(SOCK_STREAM), 
		(name ? name : "StreamClient")),
    	remoteAddr_(0),
	localAddrUpdated_(false)
{
}


StreamClientSocket::StreamClientSocket(
    const TransportAddress  & 	localAddr,
    const TransportAddress  & 	remoteAddr,
    const char      	    *	name
)
throw ( Assist::SystemException, Assist::SystemStatus )
    :	Socket( localAddr, 
    	    	SocketType(SOCK_STREAM), 
		(name ? name : "StreamClient")),
    	remoteAddr_(0),
	localAddrUpdated_(false)
{
    // Connect to the remote address, Don't catch the exception or status,
    // let it pass.
    //
    connect(remoteAddr);
}
    					

StreamClientSocket::StreamClientSocket(
    file_descriptor_t 	    	fd, 
    const TransportAddress  &   remoteAddr,
    const char      	    * 	name
)
throw ( Assist::SystemException, Assist::SystemStatus )
    :	Socket( remoteAddr.getAddressFamily(), 
    	    	SocketType(SOCK_STREAM), 
		(name ? name : "StreamClient"),
		fd),
	remoteAddr_(remoteAddr.clone()),
	localAddrUpdated_(false)
{
    // If they gave us an invalid fd descriptor, the Socket constructor
    // created one and bound it to ANY. Thus we haven't been connected,
    // so let's try to connect.
    //
    if ( fd == INVALID )
    {
    	connect(remoteAddr);
    }
}


StreamClientSocket::~StreamClientSocket()
{
}


void	
StreamClientSocket::connect(
    const TransportAddress  & 	remoteAddr
)
throw ( Assist::SystemException, Assist::SystemStatus )
{
    const string    fn("StreamClientSocket::connect");
    VLog    	    log(fn);

    if ( remoteAddr_ == 0 )
    {    
        remoteAddr_ = remoteAddr.clone();
    }

    if	(    ::connect(fd_, 
    	    	     remoteAddr_->getAddress(), 
		     remoteAddr_->getAddressLength()) < SUCCESS
	)
    {
    	int error = errno;

    	// If the socket is nonblocking and the connection will take
	// a while to complete, we'll get EINPROGRESS back. This is
	// ok, just send a status with EINPROGRESS back. The user will
	// have to use completeConnection() later on.
	//	
    	if ( error == EINPROGRESS )
	{
	    throw Assist::SystemStatus(fn + " on connect(): " + strerror(error),
	    	    	    	__FILE__, __LINE__, error);
	}

    	// Otherwise we have a problem. Signal it with an exception.
	//	
    	throw Assist::SystemException(fn + " on connect(): " + strerror(error), 
	    	    	    	__FILE__, __LINE__, error);
    }
    else
    {
    	connectionCompleted_ = true;
    }

    VDEBUG(log) << fn << ": fd = " << fd_ 
    	    	<< " connected to remote address = "
		<< *remoteAddr_ << VDEBUG_END(log);
}


void	
StreamClientSocket::completeConnection()
throw ( Assist::SystemException )
{
    if ( !remoteAddr_ )
    {
    	return;
    }
    
    const string    fn("StreamClientSocket::completeConnection");
    VLog    	    log(fn);

    int     	connectCode = 0;
    socklen_t	connectCodeSize = sizeof(connectCode);

    if ( connectionCompleted_ )
    {
    	return;
    }

    if  (   getsockopt( fd_, SOL_SOCKET, SO_ERROR, 
	    	    	#if defined(__sun) || defined(WIN32)
			(char *)
			#endif // defined(__sun)
    	    	        &connectCode, 
		        &connectCodeSize) < SUCCESS
	||  connectCode < SUCCESS 
	)
    {
    	int error = ( connectCode < SUCCESS ? connectCode : errno );
	
    	throw Assist::SystemException(fn + " on connect(): " + strerror(error), 
	    	    	    	__FILE__, __LINE__, error);
    }    	

    VDEBUG(log) << fn << ": fd = " << fd_ 
    	    	<< " connection completed to remote address = "
		<< *remoteAddr_ << VDEBUG_END(log);
}
				

int 	
StreamClientSocket::send(const string & message)
throw ( Assist::SystemException, 
    	Assist::SystemStatus, 
	Assist::Transport::ConnectionBrokenException )
{
    void * msg = reinterpret_cast<void *>(const_cast<char *>(message.c_str()));

    return ( sendMessage(msg, message.size()) );
}


int 	
StreamClientSocket::send(const char * message)
throw ( Assist::SystemException, 
    	Assist::SystemStatus, 
	Assist::Transport::ConnectionBrokenException )
{
    int messageSize = ( message ? strlen(message) : 0 );
    
    void * msg = reinterpret_cast<void *>(const_cast<char *>(message));
    
    return ( sendMessage(msg, messageSize) );
}


int 	
StreamClientSocket::send(const vector<u_int8_t> & message)
throw ( Assist::SystemException, 
    	Assist::SystemStatus, 
    	Assist::Transport::ConnectionBrokenException )
{
    void * msg = reinterpret_cast<void *>(const_cast<u_int8_t *>(&message[0]));

    return ( sendMessage(msg, message.size()) );
}


int 	
StreamClientSocket::send(const u_int8_t * message, size_t messageSize)
throw ( Assist::SystemException, 
    	Assist::SystemStatus, 
	Assist::Transport::ConnectionBrokenException )
{
    void * msg = reinterpret_cast<void *>(const_cast<u_int8_t *>(message));

    return ( sendMessage(msg, messageSize) );
}


int 	
StreamClientSocket::receive(string & message)
throw ( Assist::SystemException, 
    	Assist::SystemStatus, 
	Assist::Transport::ConnectionBrokenException )
{
    void * msg = reinterpret_cast<void *>(const_cast<char *>(message.data()));

    int bytesReceived = recvMessage(msg, message.capacity());

    message.resize(bytesReceived);
        
    return ( bytesReceived );
}


int 	
StreamClientSocket::receive(char * message, size_t messageCapacity)
throw ( Assist::SystemException, 
    	Assist::SystemStatus, 
	Assist::Transport::ConnectionBrokenException )
{
    if ( message == 0 || messageCapacity == 0 )
    {
    	return ( 0 );
    }
    
    void * msg = reinterpret_cast<void *>(message);

    int bytesReceived = recvMessage(msg, (messageCapacity > 1 ? messageCapacity-1 : 1 ));

    if ( messageCapacity > 1 )
    {    
    	message[bytesReceived] = '\0';
    }

    return ( bytesReceived );
}


int 	
StreamClientSocket::receive(vector<u_int8_t> & message)
throw ( Assist::SystemException, 
    	Assist::SystemStatus, 
	Assist::Transport::ConnectionBrokenException )
{
    message.clear();
    
    void * msg = reinterpret_cast<void *>(&message[0]);

    int bytesReceived = recvMessage(msg, message.size());

    message.resize(bytesReceived);

    return ( bytesReceived );
}
				

int 	
StreamClientSocket::receive(u_int8_t * message, size_t messageCapacity)
throw ( Assist::SystemException, 
    	Assist::SystemStatus, 
	Assist::Transport::ConnectionBrokenException )
{
    if ( message == 0 || messageCapacity == 0 )
    {
    	return ( 0 );
    }
    
    void * msg = reinterpret_cast<void *>(message);

    int bytesReceived = recvMessage(msg, messageCapacity);
    
    return ( bytesReceived );
}


Sptr<TransportAddress>
StreamClientSocket::getRemoteAddress() const
{
    return ( remoteAddr_ );
}
		

ostream &           	
StreamClientSocket::writeTo(ostream & out) const
{
    Socket::writeTo(out);
    
    if ( remoteAddr_ != 0 )
    {
    	out << ", remote = " << *remoteAddr_;
    }

    return ( out );
}

int 	
StreamClientSocket::sendMessage(
    void    	    	    *	message, 
    int     	    	    	messageLength
)
throw ( Assist::SystemException, 
    	Assist::SystemStatus, 
	Assist::Transport::ConnectionBrokenException )
{
    const string    fn("StreamClientSocket::send");
    VLog    	    log(fn);

    sockbuf_t   *   msg = static_cast<sockbuf_t *>(message);

    int bytesSent = ::send( fd_, 
    	    	    	    msg, 
			    messageLength, 
			    MSG_NOSIGNAL);
    
    if ( bytesSent < SUCCESS )
    {
    	int error = errno;
	
	// If the other side disappeared we could get an EPIPE here.
	// Issue the ConnectionBroken exception since this is a situation 
	// that needs to be cleaned up by the user.
	//
	if ( error == EPIPE )
	{
	    throw Assist::Transport::ConnectionBrokenException(
	    	    	    fn + " on send(): " + strerror(error), 
	    	    	    __FILE__, __LINE__, error);
	}
	
    	// If the socket is set nonblocking, we can get an EAGAIN
	// here, without sending the packet. If the socket is set 
	// blocking we can get an EINTR here, without sending the packet.
	// We'll throw a SystemStatus.
	// 
	if ( error == EAGAIN || error == EINTR )
	{
            throw Assist::SystemStatus(fn + " on send(): " + strerror(error), 
	    	    	    	__FILE__, __LINE__, error);
    	}	

    	throw Assist::SystemException(fn + " on send(): " + strerror(error), 
	    	    	    	__FILE__, __LINE__, error);
    }

    totalBytesSent_ += bytesSent;

    VDEBUG(log) << fn << ": send on " << *this
		<< ", bytes sent: " << bytesSent
		<< ", of total: " << messageLength
		<< VDEBUG_END(log);

    return ( bytesSent );
}
				

int 	
StreamClientSocket::recvMessage(
    void    	    	    *	message, 
    int     	    	    	messageCapacity
)
throw ( Assist::SystemException, 
    	Assist::SystemStatus, 
	Assist::Transport::ConnectionBrokenException )
{
    const string    fn("StreamClientSocket::recv");
    VLog    	    log(fn);

    sockbuf_t   *   msg = static_cast<sockbuf_t *>(message);

    int bytesReceived = ::recvfrom( fd_, 
    	    	    	    	    msg, 
				    messageCapacity, 
				    MSG_NOSIGNAL,
	    	    	    	    0, 
				    0);
				
    if ( bytesReceived <= 0 )
    {
    	int error = errno;

    	// If the other side has initiated a disconnect, we will get 0 bytes
	// received. If the other side disappeared we could get an EPIPE here.
	// Issue the ConnectionBroken exception since this is a situation 
	// that needs to be cleaned up by the user.
	//
	if ( bytesReceived == 0 || error == EPIPE )
	{
	    error = EPIPE;
	    
	    throw Assist::Transport::ConnectionBrokenException(
	    	    	    fn + " on recv(): " + strerror(error), 
	    	    	    __FILE__, __LINE__, error);
	}
	
    	// If the socket is set nonblocking, we can get an EAGAIN
	// here, without receiving the packet. If the socket is set 
	// blocking we can get an EINTR here, without receiving the packet.
	// We'll throw a SystemStatus.
	// 
	if ( error == EAGAIN || error == EINTR )
	{
            throw Assist::SystemStatus(fn + " on recv(): " + strerror(error), 
	    	    	    	__FILE__, __LINE__, error);
    	}	

    	throw Assist::SystemException(fn + " on recv(): " + strerror(error), 
	    	    	    	__FILE__, __LINE__, error);
    }

    if ( !localAddrUpdated_ )
    {
    	localAddrUpdated_ = true;
    	localAddr_->updateAddress(*this);
    }
        
    totalBytesReceived_ += bytesReceived;

    VDEBUG(log) << fn << ": received on " << *this
		<< ", bytes received: " << bytesReceived
		<< ", of capacity: " << messageCapacity
		<< VDEBUG_END(log);

    return ( bytesReceived );
};
