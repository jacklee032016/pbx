/*
* $Id: VThreadException.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include "VThreadException.hxx"

VThreadException::VThreadException( const string& msg,
                                    const string& file,
                                    const int line,
                                    const int error /*Default Argument */ )
        : VException( msg, file, line, error )
{}


VThreadExceptionInvalidAttributes::VThreadExceptionInvalidAttributes
( const string& msg,
  const string& file,
  const int line,
  const int error /*Default Argument */)
        : VThreadException( msg, file, line, error )
{
}

string
VThreadExceptionInvalidAttributes::getName( void ) const
{
    return "VThreadExceptionInvalidAttributes";
}

VThreadExceptionInvalidPriority::VThreadExceptionInvalidPriority(
    const string& msg,
    const string& file,
    const int line,
    const int error /*Default Argument*/)
        : VThreadException( msg, file, line, error )
{
}

string
VThreadExceptionInvalidPriority::getName( void ) const
{
    return "VThreadExceptionInvalidPriority";
}
