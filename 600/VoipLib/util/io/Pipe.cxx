/*
 * $Id: Pipe.cxx,v 1.2 2007/03/01 20:04:32 lizhijie Exp $
 */

#if !defined(WIN32)

#include "global.h"
#include "Pipe.hxx"
#include "VLog.hxx"
#include "SystemException.hxx"
#include "VocalCommon.hxx"
#include <string>
#include <unistd.h>
#include <cerrno>


using Vocal::IO::Pipe;
using Vocal::IO::FileDescriptor;
using Vocal::IO::file_descriptor_t;
using Vocal::Logging::VLog;
using Vocal::ReturnCode;
using Vocal::SUCCESS;


Pipe::Pipe(
    const file_descriptor_t 	*   descriptors
)
throw ( Vocal::SystemException )
    :	readFD_(FileDescriptor::INVALID),
    	writeFD_(FileDescriptor::INVALID)
{
    const string    fn("Pipe::Pipe");
    VLog    	    log(fn);
    
    VVERBOSE(log) << fn << ": this = " << this << VVERBOSE_END(log);
    
    if ( descriptors != 0 )
    {
    	readFD_.setFD(descriptors[0]);
	writeFD_.setFD(descriptors[1]);
    }
    else
    {
    	int filedes[2];
	filedes[0] = filedes[1] = FileDescriptor::INVALID;

    	ReturnCode  rc = ::pipe(filedes);
		
	if ( rc != SUCCESS )
	{
	    throw Vocal::SystemException(fn + "on pipe(): " + strerror(errno),
	    	    	    __FILE__, __LINE__, errno);
	}
	
	readFD_.setFD(filedes[0]);
	writeFD_.setFD(filedes[1]);
    }

    VDEBUG(log) << fn << ": " << *this << VDEBUG_END(log);
}


Pipe::Pipe(
    const FileDescriptor    & 	readFd,
    const FileDescriptor    &	writeFd
)
    :	readFD_(FileDescriptor::INVALID),
    	writeFD_(FileDescriptor::INVALID)
{
    const string    fn("Pipe::Pipe");
    VLog    	    log(fn);
    
    VVERBOSE(log) << fn << ": this = " << this << VVERBOSE_END(log);
    
    readFD_.setFD(readFd.getFD());
    writeFD_.setFD(writeFd.getFD());

    VDEBUG(log) << fn << ": " << *this << VDEBUG_END(log);
}	

Pipe::~Pipe()
throw ( Vocal::SystemException )
{
    const string    fn("Pipe::~Pipe");
    VLog    	    log(fn);
    
    VVERBOSE(log) << fn << ": this = " << this << VVERBOSE_END(log);
}    


FileDescriptor &    
Pipe::operator[](size_t index)
{
    if ( index == 0 )
    {
    	return ( readFD() );
    }
    
    return ( writeFD() );
}
	

FileDescriptor &    
Pipe::readFD()
{
    return ( readFD_ );
}


const FileDescriptor &    
Pipe::readFD() const
{
    return ( readFD_ );
}


FileDescriptor &    
Pipe::writeFD()
{
    return ( writeFD_ );
}


const FileDescriptor &    
Pipe::writeFD() const
{
    return ( writeFD_ );
}


bool	
Pipe::operator==(const Pipe & src) const
{
    return ( readFD_ == src.readFD_ && writeFD_ == src.writeFD_ );
}


bool	
Pipe::operator<(const Pipe & src) const
{
    return  ( 	readFD_ < src.readFD_ 
    	    ||	(   readFD_ == src.readFD_ && writeFD_ < src.writeFD_ )
	    );
}


ostream &       
Pipe::writeTo(ostream & out) const
{
    return  ( 	out <<"pipe = { read_" << readFD_
	    	    << ", write_" << writeFD_ << " }"
	    );
}
#endif
