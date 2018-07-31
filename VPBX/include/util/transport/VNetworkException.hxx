#ifndef VNetworkException_hxx
#define VNetworkException_hxx
/*
* $Id: VNetworkException.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
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
