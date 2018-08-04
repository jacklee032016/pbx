/*
* $Id: SystemStatus.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include "SystemStatus.hxx"
#include "VLog.hxx"


using Assist::SystemStatus;
using Assist::Logging::VLog;


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
