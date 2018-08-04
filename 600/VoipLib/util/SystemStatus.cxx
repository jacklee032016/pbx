/*
 * $Id: SystemStatus.cxx,v 1.2 2007/03/01 20:04:32 lizhijie Exp $
 */


#include "global.h"
#include "SystemStatus.hxx"
#include "VLog.hxx"


using Vocal::SystemStatus;
using Vocal::Logging::VLog;


SystemStatus::SystemStatus(  
    const string	    & 	msg,
    const string    	    & 	file,
    const int 	    	    	line,
    const int 	    	    	status
)
    :	VException(msg, file, line, status)
{
    VLog    log("SystemStatus::SystemStatus");
    
    VDEBUG(log) << msg 
    		<< ", file = " 	    << file 
	    	<< ", line = " 	    << line
		<< ", status = "    << status
		<< VDEBUG_END(log);
}
    

string      	    	    
SystemStatus::getName() const
{
    return ( "SystemStatus" );
}


int   	    	    	    
SystemStatus::getStatus() const
{
    return ( errorCode );
}
