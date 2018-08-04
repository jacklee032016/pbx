#ifndef DATA_EXCEPTION_HXX_
#define DATA_EXCEPTION_HXX_
/*
 * $Id: DataException.hxx,v 1.2 2007/03/01 20:04:29 lizhijie Exp $
 */


#include "global.h"
#include <string>
#include "VException.hxx"

/** Exception handling class DataException.  None of this is used --
 * this class is depreciated and will be removed or renovated in the
 * future 
 */
class DataException : protected VException
{
    public:
        ///
        DataException( const string& msg,
                       const string& file,
                       const int line,
                       const int error = 0 );

        ///
        string getName( void ) const;
};

#endif
