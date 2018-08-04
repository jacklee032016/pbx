/*
* $Id: CpPriorityLog.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include "CpPriorityLog.hxx"
#include "Lock.hxx"
#include <cstdarg>


using namespace Assist;
using Logging::PriorityLog;
using Logging::CpPriorityLog;
using Process::Lock;


VMutex	    	    CpPriorityLog::writeMutex_;


static void
S_cpLog(int pri, const char* file, int line, const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vCpLog(pri, file, line, fmt, ap);
    va_end(ap);
}


CpPriorityLog::CpPriorityLog(
    const char  	*   priorityName,
    int 	    	    priority,
    bool	    	    alwaysOn,
    bool	    	    defaultOn
)
    :	PriorityLog(priorityName, priority, alwaysOn, defaultOn)
{
}


CpPriorityLog::CpPriorityLog(const PriorityLog & src)
    :	PriorityLog(src)
{
}


CpPriorityLog::~CpPriorityLog()
{
}


PriorityLog *
CpPriorityLog::clone() const
{
    return ( new CpPriorityLog(*this) );
}


void	    	    
CpPriorityLog::writeLog(
    const string	&,
    int 	    	    priority,
    const char  	*   logMessage,
    const char  	*   file, 
    int 	    	    line
)
{
    Lock    lock(writeMutex_); (void)lock;

    S_cpLog(priority, file, line, "%s", logMessage);
}
