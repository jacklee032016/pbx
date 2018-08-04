/*
* $Id: DataException.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
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
