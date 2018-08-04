/*
 * $Id: ConnectionBrokenException.cxx,v 1.2 2007/03/01 20:04:33 lizhijie Exp $
 */

#include "global.h"
#include "ConnectionBrokenException.hxx"


using Vocal::SystemException;
using Vocal::Transport::ConnectionBrokenException;


ConnectionBrokenException::ConnectionBrokenException(  
    const string	    & 	msg,
    const string    	    & 	file,
    const int 	    	    	line,
    const int 	    	    	error,
    bool    	    	    	warning
)
    :	SystemException(msg, file, line, error, warning)
{
}


string      	    	    
ConnectionBrokenException::getName() const
{
    return ( "ConnectionBrokenException");
}
