/*
 * $Id: DataException.cxx,v 1.2 2007/03/01 20:04:32 lizhijie Exp $
 */


#include "global.h"
#include "DataException.hxx"


DataException::DataException( const string& msg,
                              const string& file,
                              const int line,
                              const int error /*Default Argument*/ )
        : VException( msg, file, line, error )
{}



string
DataException::getName( void ) const
{
    return "DataException";
}
