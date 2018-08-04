#ifndef VEXCEPTION_HXX
#define VEXCEPTION_HXX

/*
* $Id: VException.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"
#include <exception>
#include <string>
#include <iostream>

/** Assist specific exception.
 */
class VException : public std::exception
{
    protected:

        /** Creates an exception object, should be called with a
         ** descriptive msg, the filename and line number where the
         ** exception occured, and optionally an error code asscociated
         ** with the exception
         ** Note, the log() method handles the logging format, so
         ** blank spaces, newlines, etc. should NOT be included in the
         ** parameters to the constructor.
         */
        VException( const string& msg,
                    const string& file,
                    const int line,
                    const int error = 0);

        /** destroys the exception object
         */
        ~VException() throw();

        /** Returns the predefined name of the exception
         */
        virtual string getName() const = 0;

        /** Logs the exception including the error code (if any) as well
         ** as the filename and line number where the exception
         ** occurred.
         **
         ** The logging is formatted as follows:
         ** <msg> [:<error>] at <file>:<line> 
         */
        void log() const;

    public:

        /** Returns the predefined name and user supplied msg
         */
        string getDescription() const;

        /** Returns the error code supplied with the exception.
         */
        int getError() const;

    protected:
        /// user supplied msg
        string message;

        /// file in which exception occurred
        string fileName;

        /// line number at which exception occurred
        int lineNumber;

        /// user supplied error code (optional)
        int errorCode;
};

ostream& operator<<(ostream& strm, const VException& e);

/** Generic exception classes
 *  Note, class specific exception classes should be defined in
 *  separate <ClassName>Exception.[ch]xx files
 */
class VExceptionMemory: protected VException
{
    public:
        VExceptionMemory( const string& msg,
                          const string& file,
                          const int line,
                          const int error = 0 );

        string getName( void ) const;
};


// VEXCEPTION_HXX
#endif
