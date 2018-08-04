#ifndef VNetworkException_hxx
#define VNetworkException_hxx
/*
 * $Id: VNetworkException.hxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */


#include "VException.hxx"

/// VNetworkException
class VNetworkException: public VException
{
    public:

        VNetworkException( const string& msg,
                           const string& file,
                           const int line,
                           const int error = 0 )
        : VException(msg, file, line, error)
        { }

        ///
        string getName( void ) const
        {
            return "VNetworkException";
        }
};

#endif
