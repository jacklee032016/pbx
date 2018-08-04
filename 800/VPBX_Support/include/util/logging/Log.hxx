#if !defined(LOG_DOT_H)
#define LOG_DOT_H
/*
* $Id: Log.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include <map>
#include "Sptr.hxx"
#include "PriorityLog.hxx"


/** Infrastructure common to ASSIST.
 */
namespace Assist 
{


/** Infrastructure in ASSIST related to logging.<br><br>
 */
namespace Logging
{


/** Aggregator of PriorityLogs.<br><br>
 *
 *  @see    Assist::Logging::PriorityLog
 *  @see    Assist::Logging::VLog
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
} // namespace Assist


#endif // !defined(LOG_DOT_H)
