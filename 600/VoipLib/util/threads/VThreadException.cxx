/*
 * $Id: VThreadException.cxx,v 1.2 2007/03/01 20:04:33 lizhijie Exp $
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
