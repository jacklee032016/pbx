/*
* $Id: ConnectionBrokenException.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include "ConnectionBrokenException.hxx"


using Assist::SystemException;
using Assist::Transport::ConnectionBrokenException;


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
