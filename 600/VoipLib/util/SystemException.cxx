/*
 * $Id: SystemException.cxx,v 1.2 2007/03/01 20:04:32 lizhijie Exp $
 */


#include "global.h"
#include "SystemException.hxx"
#include "VLog.hxx"


using Vocal::SystemException;
using Vocal::Logging::VLog;


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
