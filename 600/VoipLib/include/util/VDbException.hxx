#ifndef VDbException_hxx
#define VDbException_hxx
/*
 * $Id: VDbException.hxx,v 1.2 2007/03/01 20:04:29 lizhijie Exp $
 */

#include "VException.hxx"

/// VDbException
class VDbException: public VException
{
    public:

        VDbException( const string& msg,
                               const string& file,
                               const int line,
                               const int error = 0 )
        : VException(msg, file, line, error)
        { }
        ;

        ///
        string getName( void ) const
        {
            return "VDbException";
        }
};

#endif
