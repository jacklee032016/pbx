/*
 * $Id: Log.cxx,v 1.2 2007/03/01 20:04:32 lizhijie Exp $
 */


#include "global.h"
#include "Log.hxx"


using Vocal::Logging::Log;
using Vocal::Logging::PriorityLog;


Log::LogMap	Log::log_;


Log::Log()
{
}


Log::~Log()
{
}


void	    	    
Log::registerLog(int priority, Sptr<PriorityLog> priorityLog)
{
    if ( priorityLog != 0 )
    {
        log_[priority] = priorityLog;
    }
}


Sptr<PriorityLog>
Log::getLog(int priority)
{
    LogMap::iterator	it = log_.find(priority);
    Sptr<PriorityLog> nullLog = 0, log;

    log = ( it == log_.end() ? nullLog : (*it).second );

    return ( (log != 0 && log->isOn()) ? log : nullLog );
}


void
Log::clearLogs()
{
    log_.clear();
}
	
