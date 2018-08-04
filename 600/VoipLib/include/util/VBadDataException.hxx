#ifndef VBadDataException_hxx
#define VBadDataException_hxx
/*
 * $Id: VBadDataException.hxx,v 1.2 2007/03/01 20:04:29 lizhijie Exp $
 */

#include "VException.hxx"

/// VBadDataException
class VBadDataException: public VException
{
    public:

        VBadDataException( const string& msg,
                           const string& file,
                           const int line,
                           const int error = 0 )
        : VException(msg, file, line, error)
        { }
        ;

        ///
        string getName( void ) const
        {
            return "VBadDataException";
        }
};

#endif
