/*
* $Id: Socket.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include "Socket.hxx"
#include "TransportCommon.hxx"
#include "SystemException.hxx"
#include "TransportAddress.hxx"
#include "SocketType.hxx"
#include "VLog.hxx"
#include <cerrno>
#include <sys/types.h>


using Assist::IO::file_descriptor_t;
using Assist::IO::FileDescriptor;
using Assist::Transport::Socket;
using Assist::Transport::TransportAddress;
using Assist::Transport::AddressFamily;
using Assist::Transport::SocketType;
using Assist::Logging::VLog;


Socket::Socket(
    const AddressFamily     &	addressFamily,
    const SocketType 	    &	socketType,
    const char	    	    *	name,
    file_descriptor_t	    	fd
)
throw ( Assist::SystemException )
    :	FileDescriptor(fd),
    	localAddr_(TransportAddress::create(addressFamily())),
    	addressFamily_(addressFamily),
    	socketType_(socketType),
	totalBytesSent_(0),
	totalBytesReceived_(0),
	name_(name ? name : "")
{
    const string    fn("Socket::Socket");
    VLog    	    log(fn);

    // Finish the name.
    //
    name_ += "Socket";
        
    // File descriptor may have been passed in already.
    // If so, assume it has already been bound.
    //
    if ( fd_ == INVALID )
    {
        fd_ = socket(addressFamily(), socketType_(), 0);
    
        if ( fd_ <= INVALID )
        {
        	fd_ = INVALID;
        	throw Assist::SystemException(fn + " on socket(): " + strerror(errno), 
    	    	    	    	    	__FILE__, __LINE__, errno);
    	}
    
	// Bind the socket.
	//
	if  (   bind(fd_, 
    	    	     localAddr_->getAddress(), 
		     localAddr_->getAddressLength()) < SUCCESS
    	    )
	{
	    close(); 
    	    throw Assist::SystemException(fn + " on bind(): " + strerror(errno), 
	    	    	    	     __FILE__, __LINE__, errno);
	}
    }
        
    // Update the transport address. Don't catch the exception, let it pass.
    //
    localAddr_->updateAddress(*this);

    // Report back, for the curious among us.
    //
    VDEBUG(log) << fn << ": "  << *this << VDEBUG_END(log);
}


Socket::Socket(
    const TransportAddress  &   localAddr,
    const SocketType 	    &   socketType,
    const char	    	    *	name
)   	
throw ( Assist::SystemException )
    :	localAddr_(localAddr.clone()),
    	addressFamily_(localAddr.getAddressFamily()),
    	socketType_(socketType),
	totalBytesSent_(0),
	totalBytesReceived_(0),
	name_(name ? name : "")
{
    const string    fn("Socket::Socket");
    VLog    	    log(fn);
    
    // Finish the name.
    //
    name_ += "Socket";
        
    // Create the socket.
    //
    AddressFamily   	addressType = localAddr_->getAddressFamily();

    fd_ = socket(addressType(), socketType_(), 0);
    
    if ( fd_ <= INVALID )
    {
    	fd_ = INVALID;
    	throw Assist::SystemException(fn + " on socket(): " + strerror(errno), 
	    	    	    	__FILE__, __LINE__, errno);
    }
    
    // Bind the socket.
    //
    if  (   bind(fd_, 
    	    	 localAddr_->getAddress(), 
		 localAddr_->getAddressLength()) < SUCCESS
    	)
    {
    	close();
        VWARN(log)  << fn << ": Could not bind to address: " << *localAddr_
                    << VWARN_END(log);
                    
    	throw Assist::SystemException(fn + " on bind(): " + strerror(errno), 
	    	    	    	 __FILE__, __LINE__, errno);
    }
    
    // Update the transport address. Don't catch the exception, let it pass.
    //
    localAddr_->updateAddress(*this);
    
    // Report back, for the curious among us.
    //
    VDEBUG(log) << fn << ": "  << *this << VDEBUG_END(log);
}
    

Socket::~Socket()
{
    VLog    log("Socket::~Socket");

    delete localAddr_;
}


const 	TransportAddress &
Socket::getLocalAddress() const
{
    return ( *localAddr_ );
}


const 	AddressFamily &
Socket::getAddressFamily() const
{
    return ( addressFamily_ );
}


const	SocketType &	    	
Socket::getSocketType() const
{
    return ( socketType_ );
}
    

unsigned long	    	
Socket::bytesSent() const
{
    return ( totalBytesSent_ );
}


unsigned long	    	
Socket::bytesReceived() const
{
    return ( totalBytesReceived_ );
}


ostream &       
Socket::writeTo(ostream & out) const
{
    out << name_ 
    	<< ": protocol = "	<< addressFamily_
    	<< ", type = "  	<< socketType_
	<< ", ";

    FileDescriptor::writeTo(out);
    		
    out << ", local = " 	<< *localAddr_ 
	<< ", sent = "  	<< totalBytesSent_
	<< ", received = "	<< totalBytesReceived_;
	
    return ( out );
}
