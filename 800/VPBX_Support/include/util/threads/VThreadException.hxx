#ifndef VTHREAD_EXCEPTION_HXX
#define VTHREAD_EXCEPTION_HXX
/*
* $Id: VThreadException.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "VException.hxx"

/// VThread base exception class
class VThreadException: public VException
{
    protected:
	/// constructor
        VThreadException( const string& msg,
                          const string& file,
                          const int line,
                          const int error = 0 );

        /// get the exception name
        virtual string getName( void ) const = 0;
};


/// invalid attributes have been passed to some VThread method
class VThreadExceptionInvalidAttributes: protected VThreadException
{
    public:
	/// constructor
        VThreadExceptionInvalidAttributes( const string& msg,
                                           const string& file,
                                           const int line,
                                           const int error = 0 );
	/// get the exception name
        string getName( void ) const;
};


/// invalid priorities have been passed to some VThread method
class VThreadExceptionInvalidPriority: protected VThreadException
{
    public:
	/// constructor
        VThreadExceptionInvalidPriority( const string& msg,
                                         const string& file,
                                         const int line,
                                         const int error = 0 );
	/// set the exception name
        string getName( void ) const;
};

// VTHREAD_EXCEPTION_HXX_
#endif
