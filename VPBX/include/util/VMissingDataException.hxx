#ifndef VMissingDataException_hxx
#define VMissingDataException_hxx

/*
* $Id: VMissingDataException.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
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
