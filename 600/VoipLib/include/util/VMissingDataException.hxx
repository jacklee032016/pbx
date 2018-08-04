#ifndef VMissingDataException_hxx
#define VMissingDataException_hxx
/*
 * $Id: VMissingDataException.hxx,v 1.2 2007/03/01 20:04:29 lizhijie Exp $
 */


#include "VException.hxx"

/// VMissingDataException
class VMissingDataException: public VException
{
    public:

        VMissingDataException( const string& msg,
                               const string& file,
                               const int line,
                               const int error = 0 )
        : VException(msg, file, line, error)
        { }
        ;

        ///
        string getName( void ) const
        {
            return "VMissingDataException";
        }
};

#endif
