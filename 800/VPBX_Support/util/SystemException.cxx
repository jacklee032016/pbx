/*
* $Id: SystemException.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include "SystemException.hxx"
#include "VLog.hxx"


using Assist::SystemException;
using Assist::Logging::VLog;


SystemException::SystemException(  
    const string	    & 	msg,
    const string    	    & 	file,
    const int 	    	    	line,
    const int 	    	    	error,
    bool    	    	    	warning
)
    :	VException(msg, file, line, error)
{
    VLog    log("SystemException::SystemException");
    
    if ( warning )
    {
        VWARN(log)  << msg 
    		    << ", file = " 	<< file 
	    	    << ", line = " 	<< line
		    << ", error = "	<< error 
		    << VWARN_END(log);
    }
}


string      	    	    
SystemException::getName() const
{
    return ( "SystemException" );
}


int
SystemException::getError() const
{
    return ( errorCode );
}
