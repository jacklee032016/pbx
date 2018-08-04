#if !defined(LOG_DOT_H)
#define LOG_DOT_H
/*
 * $Id: Log.hxx,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */


#include <map>
#include "Sptr.hxx"
#include "PriorityLog.hxx"


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Infrastructure in VOCAL related to logging.<br><br>
 */
namespace Logging
{


/** Aggregator of PriorityLogs.<br><br>
 *
 *  @see    Vocal::Logging::PriorityLog
 *  @see    Vocal::Logging::VLog
 */
class Log
{
    protected:

    	/** Desfault constructor.
	 */
	Log();


    public:


    	/** Virtual destructor.
	 */
	virtual ~Log();
	

    protected:
    
    	/** Class method for adding a PriorityLog
	 *  to the store.
	 */
    	static	void	    	    registerLog(int priority, Sptr<PriorityLog>);


    	/** Class method for getting a PriorityLog from
	 *  the store.
	 */
	static	Sptr<PriorityLog>   getLog(int priority);


    	/** Clears the store of all PriorityLogs.
	 */
	static	void	    	    clearLogs();
	
	typedef map<int, Sptr<PriorityLog> >   	LogMap;
	static	LogMap	    	    	    	log_;
};


} // namespace Logging
} // namespace Vocal


#endif // !defined(LOG_DOT_H)
