/*
* $Id: PriorityLog.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include "PriorityLog.hxx"
#include "VThread.hxx"
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <cstdio>
#include <iomanip>


using Assist::Logging::PriorityLog;
using Assist::Logging::setFile;
using Assist::Logging::setLine;


PriorityLog::PriorityLog(
    const char *	    priorityName,
    int 	    	    priority,
    bool	    	    alwaysOn,
    bool	    	    defaultOn
)
    :	buffer_(0),
    	priorityName_(priorityName),
	priority_(priority),
	alwaysOn_(alwaysOn),
	on_(alwaysOn ? true : defaultOn),
        file_(""),
        line_(0)
{
}


PriorityLog::PriorityLog(const PriorityLog & src)
    :	buffer_(0),
    	priorityName_(src.priorityName_),
	priority_(src.priority_),
	alwaysOn_(src.alwaysOn_),
	on_(src.on_),
        file_(src.file_),
        line_(src.line_)
{
}


PriorityLog &
PriorityLog::operator=(const PriorityLog & src)
{
    if ( this != & src )
    {
        delete buffer_;
        buffer_ = 0;
        
    	priorityName_ = src.priorityName_;
	priority_ = src.priority_;
	alwaysOn_ = src.alwaysOn_;
	on_ = src.on_;
        file_ = src.file_;
        line_ = src.line_;
    }

    return ( *this );        
}


PriorityLog::~PriorityLog()
{
    delete buffer_;
}


ostream &	    
PriorityLog::get(bool header)
{
    if ( !on_ )
    {
    	return ( cerr );
    }
    
    if ( !buffer_ )
    {
    	buffer_ = new ostrstream;
    }
    
    if ( header )
    {
    	preWrite();
    }
    
    return ( *buffer_ );
}


bool	    	    
PriorityLog::isOn() const
{
    return ( on_ );
}


void	    	    
PriorityLog::on()
{
    on_ = true;
}


void	    	    
PriorityLog::off()
{
    if ( !alwaysOn_ )
    {
    	on_ = false;
    }
}


void	    	    
PriorityLog::preWrite()
{
}


void	    	    
PriorityLog::postWrite()
{
}


void                
PriorityLog::file(const char * file__)
{
    if ( file__ )
    {
        file_ = file__;
    }
}


void                
PriorityLog::line(int line__)
{
    line_ = line__;
}
                

void	    	    
PriorityLog::end()
{
    if ( !on_ || !buffer_ )
    {
    	return;
    }
    
    *buffer_ << ends;
    const char *    logMessage = buffer_->str();

    preWrite();
                
    writeLog(priorityName_, priority_, logMessage, file_.c_str(), line_);
    
    postWrite();
    
    buffer_->freeze(false);
    delete buffer_;
    buffer_ = 0;
}
		

ostream & 
PriorityLog::writeTo(ostream & out) const
{
    PriorityLog     & priorityLog = *const_cast<PriorityLog *>(this);
    priorityLog.end();

    return ( out );
}


setFile::setFile(PriorityLog & priorityLog, const char * fileName)
{
    priorityLog.file(fileName);
}


ostream & 
setFile::writeTo(ostream & out) const
{
    return ( out );
}        


setLine::setLine(PriorityLog & priorityLog, int fileLine)
{
    priorityLog.line(fileLine);
}

ostream & 
setLine::writeTo(ostream & out) const
{
    return ( out );
}
        
