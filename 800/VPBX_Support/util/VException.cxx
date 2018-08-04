/*
* $Id: VException.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include "cpLog.h"
#include "VException.hxx"

VException::VException( const string& msg,
                        const string& file,
                        const int line,
                        const int error /*Default Argument*/):
        message( msg ),
        fileName( file ),
        lineNumber( line ),
        errorCode( error )
{
   cpLog(LOG_DEBUG, "Exception at %s:%d %s", file.c_str(), line, message.c_str());
}


VException::~VException()
    throw()
{
}

string
VException::getDescription( void ) const
{
    return ( getName() + ": " + message );
}

int
VException::getError() const
{
    return ( errorCode );
}

void
VException::log( void ) const
{
    if ( errorCode )
    {
        cpLog( LOG_DEBUG,
               "%s: %d at %s:%d\n",
               getDescription().c_str(),
               errorCode,
               fileName.c_str(),
               lineNumber );
    }
    else
    {
        cpLog( LOG_DEBUG,
               "%s at %s:%d\n",
               getDescription().c_str(),
               fileName.c_str(),
               lineNumber );
    }
}

ostream& operator<<(ostream& strm, const VException& e)
{
   strm << e.getDescription();
   return strm;
}

VExceptionMemory::VExceptionMemory(
    const string& msg,
    const string& file,
    const int line,
    const int error /*Default Argument*/)
        : VException( msg, file, line, error )
{
    log();
}

string
VExceptionMemory::getName( void ) const
{
    return "VExceptionMemory";
}
