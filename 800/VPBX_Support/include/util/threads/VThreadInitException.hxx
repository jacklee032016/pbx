#ifndef VThreadInitException_hxx
#define VThreadInitException_hxx
/*
* $Id: VThreadInitException.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "VThreadException.hxx"

/// class that defines an exception upon initialization of a thread.
class VThreadInitException: public VThreadException
{
    public:
	/// constructor
        VThreadInitException( const string& msg,
                              const string& file,
                              const int line,
                              const int error = 0 )
        : VThreadException(msg, file, line, error)
        { }
        ;

	/// get the name of the exception
        string getName( void ) const
        {
            return "VThreadInitException";
        };
};

#endif
