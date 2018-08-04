/*
 * $Id: FileDescriptor.cxx,v 1.2 2007/03/01 20:04:32 lizhijie Exp $
 */

#include "FileDescriptor.hxx"
#include "VLog.hxx"
#include "SystemException.hxx"
#include "SystemStatus.hxx"
#include "VocalCommon.hxx"
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>

using Vocal::IO::FileDescriptor;
using Vocal::IO::file_descriptor_t;
using Vocal::Logging::VLog;
using Vocal::SUCCESS;


const file_descriptor_t	    FileDescriptor::INVALID = -1;


FileDescriptor::FileDescriptor(file_descriptor_t fd)
    :	fd_(fd)
{
}


FileDescriptor::~FileDescriptor()
{
    if ( fd_ != INVALID )
    {
        try
        {
    	    close();
        }
        catch ( ... )
        {
        }
    }
}


void	
FileDescriptor::setFD(file_descriptor_t fd)
{
    if ( fd_ != INVALID )
    {
    	close();
    }
    fd_ = fd;
}


file_descriptor_t   	
FileDescriptor::getFD() const
{
    return ( fd_ );
}


void  	    	
FileDescriptor::setBlocking()
throw ( Vocal::SystemException )
{
    const string    fn("FileDescriptor::setBlocking");
    VLog    	    log(fn);

#if defined(WIN32)
    VERR(log) << "not implemented under Win32" << VERR_END(log);
    assert(0);
#else
    
    int flags = fcntl(fd_, F_GETFL);
    
    if	(   flags < 0 
    	||  fcntl(fd_, F_SETFL, flags & ~O_NONBLOCK) <  0
    	)
    {
    	throw Vocal::SystemException(fn + " on fcntl(): " + strerror(errno), 
	    	    	    	__FILE__, __LINE__, errno);
    }

    VDEBUG(log) << (fn + ": fd = ") << fd_ << VDEBUG_END(log);
#endif
}


void  	    	
FileDescriptor::setNonblocking()
throw ( Vocal::SystemException )
{
    const string    fn("FileDescriptor::setNonblocking");
    VLog    	    log(fn);

#if defined(WIN32)
    VERR(log) << "not implemented under Win32" << VERR_END(log);
    assert(0);
#else
    int flags = fcntl(fd_, F_GETFL);
    
    if  (   flags < 0 
    	||  fcntl(fd_, F_SETFL, flags | O_NONBLOCK) < 0
    	)
    {
    	throw Vocal::SystemException(fn + " on fcntl(): " + strerror(errno), 
	    	    	    	__FILE__, __LINE__, errno);
    }

    VDEBUG(log) << (fn + ": fd = ") << fd_ << VDEBUG_END(log);
#endif
}


int 	
FileDescriptor::write(const string & stuff)
throw ( Vocal::SystemException, Vocal::SystemStatus )
{
    if ( stuff.size() == 0 )
    {
    	return ( 0 );
    }
    
    void * s = reinterpret_cast<void *>(const_cast<char *>(stuff.c_str()));
    
    return ( writeToFD(s, stuff.size()) );
}


int 	
FileDescriptor::write(const char * stuff)
throw ( Vocal::SystemException, Vocal::SystemStatus )
{
    size_t  size = ( stuff ? strlen(stuff) : 0 );
    
    if ( size == 0 )
    {
    	return ( 0 );
    }

    void * s = reinterpret_cast<void *>(const_cast<char *>(stuff));
    
    return ( writeToFD(s, size) );
}


int 	
FileDescriptor::write(const vector<u_int8_t> & stuff)
throw ( Vocal::SystemException, Vocal::SystemStatus )
{
    if ( stuff.size() == 0 )
    {
    	return ( 0 );
    }

    void * s = reinterpret_cast<void *>(const_cast<u_int8_t *>(&stuff[0]));

    return ( writeToFD(s, stuff.size()) );
}


int 	
FileDescriptor::write(const u_int8_t * stuff, size_t size)
throw ( Vocal::SystemException, Vocal::SystemStatus )
{
    if ( stuff == 0 || size == 0 )
    {
    	return ( 0 );
    }

    void * s = reinterpret_cast<void *>(const_cast<u_int8_t *>(stuff));

    return ( writeToFD(s, size) );
}


int 	
FileDescriptor::read(string & stuff)
throw ( Vocal::SystemException, Vocal::SystemStatus )
{
    void * s = reinterpret_cast<void *>(const_cast<char *>(stuff.data()));

    int bytesRead = readFromFD(s, stuff.size());

    stuff.resize(bytesRead);
        
    return ( bytesRead );
}


int 	
FileDescriptor::read(char * stuff, size_t capacity)
throw ( Vocal::SystemException, Vocal::SystemStatus )
{
    if ( stuff == 0 || capacity == 0 )
    {
    	return ( 0 );
    }
    
    void * s = reinterpret_cast<void *>(stuff);

    int bytesRead = readFromFD(s, (capacity > 1 ? capacity-1 : 1 ) );

    if ( capacity > 1 )
    {    
    	stuff[bytesRead] = '\0';
    }

    return ( bytesRead );
}


int 	
FileDescriptor::read(vector<u_int8_t> & stuff)
throw ( Vocal::SystemException, Vocal::SystemStatus )
{
    stuff.clear();
    
    void * s = reinterpret_cast<void *>(&stuff[0]);

    int bytesRead = readFromFD(s, stuff.size());

    stuff.resize(bytesRead);
    
    return ( bytesRead );
}


int 	
FileDescriptor::read(u_int8_t * stuff, size_t capacity) 
throw ( Vocal::SystemException, Vocal::SystemStatus )
{
    if ( stuff == 0 || capacity == 0 )
    {
    	return ( 0 );
    }
    
    void * s = reinterpret_cast<void *>(stuff);

    int bytesRead = readFromFD(s, capacity);
    
    return ( bytesRead );
}


void  	    	
FileDescriptor::close()
throw ( Vocal::SystemException )
{
    const string    fn("FileDescriptor::close");
    VLog    	    log(fn);

    #ifndef WIN32
    if ( ::close(fd_) < 0 )
    #else
    if ( ::closesocket(fd_) == SOCKET_ERROR )
    #endif
    {
    	fd_ = INVALID;
    	throw Vocal::SystemException(fn + " on std::close(): " + strerror(errno), 
	    	    	    	__FILE__, __LINE__, errno);
    }

    VDEBUG(log) << (fn + ": fd = ") << fd_ << VDEBUG_END(log);

    fd_ = INVALID;
}


bool	    	    
FileDescriptor::operator==(const FileDescriptor & rhs) const
{
    return ( fd_ == rhs.fd_ );
}


bool	    	    
FileDescriptor::operator!=(const FileDescriptor & rhs) const
{
    return ( fd_ != rhs.fd_ );
}


bool	    	    
FileDescriptor::operator<(const FileDescriptor & rhs) const
{
    return ( fd_ < rhs.fd_ );
}


bool	    	    
FileDescriptor::operator<=(const FileDescriptor & rhs) const
{
    return ( fd_ <= rhs.fd_ );
}


bool	    	    
FileDescriptor::operator>(const FileDescriptor & rhs) const
{
    return ( fd_ > rhs.fd_ );
}


bool	    	    
FileDescriptor::operator>=(const FileDescriptor & rhs) const
{
    return ( fd_ >= rhs.fd_ );
}


ostream &       
FileDescriptor::writeTo(ostream & out) const
{
    return ( out << "fd = " << fd_ );
}


int 	
FileDescriptor::writeToFD(void * stuff, int size)
throw ( Vocal::SystemException, Vocal::SystemStatus )
{
    const string    fn("FileDescriptor::writeToFD");
    VLog    	    log(fn);

    int bytesWritten = 0;
 
#ifndef WIN32
    bytesWritten = ::write( fd_, 
	    	    	    stuff, 
			    size);

    if ( bytesWritten < SUCCESS )
    {
    	int error = errno;
	
    	// If the socket is set nonblocking, we can get an EAGAIN
	// here, without sending the packet. If the socket is set 
	// blocking we can get an EINTR here, without sending the packet.
	// 
	if ( error == EAGAIN || error == EINTR )
	{
    	    throw Vocal::SystemStatus(fn + " on write(): " + strerror(error),
	    	    	    	__FILE__, __LINE__, error);
    	}	

    	throw Vocal::SystemException(fn + " on write(): " + strerror(error), 
	    	    	    	__FILE__, __LINE__, error);
    }

    VDEBUG(log) << fn << ": written on " << *this
		<< ", bytes written: " << bytesWritten
		<< ", of total: " << size
		<< VDEBUG_END(log);
#else
    // TODO
    assert(0);
#endif

    return ( bytesWritten );
}
				

int 	
FileDescriptor::readFromFD(void * stuff, int capacity)
throw ( Vocal::SystemException, Vocal::SystemStatus )
{
    const string    fn("FileDescriptor::readFromFD");
    VLog    	    log(fn);

    int bytesRead = 0;

#ifndef WIN32
    bytesRead = ::read( fd_, 
	    	    	stuff, 
			capacity);

    if ( bytesRead < SUCCESS )
    {
    	int error = errno;
	
    	// If the socket is set nonblocking, we can get an EAGAIN
	// here, without receiving the packet. If the socket is set 
	// blocking we can get an EINTR here, without receiving the packet.
	// 
	if ( error == EAGAIN || error == EINTR )
	{
    	    throw Vocal::SystemStatus(fn + " on read(): " + strerror(error),
	    	    	    	__FILE__, __LINE__, error);
    	}	

    	throw Vocal::SystemException(fn + " on read(): " + strerror(error), 
	    	    	    	__FILE__, __LINE__, error);
    }

    VDEBUG(log) << fn << ": read on " << *this
		<< ", bytes read: " << bytesRead
		<< ", of capacity: " << capacity
		<< VDEBUG_END(log);
#else
    // TODO
    assert(0);
#endif

    return ( bytesRead );
}


