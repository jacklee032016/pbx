#ifndef VIoException_hxx
#define VIoException_hxx

/*
 * $Id: VIoException.hxx,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */


#include "VException.hxx"

/// VIoException
class VIoException: public VException
{
    public:

        VIoException( const string& msg,
                      const string& file,
                      const int line,
                      const int error = 0 )
        : VException(msg, file, line, error)
        { }
        ;

        ///
        string getName( void ) const
        {
            return "VIoException";
        }
};

#endif
