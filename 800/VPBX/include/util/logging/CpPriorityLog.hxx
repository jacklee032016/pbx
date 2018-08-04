#if !defined(CP_PRIORITY_LOG_DOT_H)
#define CP_PRIORITY_LOG_DOT_H
/*
* $Id: CpPriorityLog.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "PriorityLog.hxx"
#include "cpLog.h"
#include "VMutex.h"


/** Infrastructure common to ASSIST.
 */
namespace Assist 
{


/** Infrastructure in ASSIST related to logging.<br><br>
 */
namespace Logging
{


/** Assist specific extention of the PriorityLog class.<br><br>
 *
 *  This implements a PriorityLog using the vocal/util/cpLog facility.
 *
 *  @see    Assist::Logging::PriorityLog
 *  @see    Assist::Logging::Log
 *  @see    Assist::Logging::VLog
 *  @see    VMutex
 */
class CpPriorityLog : public Assist::Logging::PriorityLog
{
    public:


    	/** Create a CpPriorityLog. All parameters are passed to PriorityLog.
	 */
    	CpPriorityLog(
	    const char  	*   priorityName,
	    int 	    	    priority,
	    bool	    	    alwaysOn	= false,
	    bool	    	    defaultOn 	= true);


    	/** Copy constructor.
	 */
	CpPriorityLog(const PriorityLog &);


    	/** Virtual destructor.
	 */
	virtual ~CpPriorityLog();


    	/** Create a CpPriorityLog copy of this object.
	 */
    	virtual PriorityLog *	    clone() const;


    	/** Write the given log message to the cpLog.
	 */
	virtual void	    	    writeLog(
	    	    	    	    	const string	&   priorityName,
					int 	    	    priority,
					const char  	*   logMessage,
					const char  	*   file = 0, 
					int 	    	    line = 0);
					
    private:


    	static	VMutex	    	    writeMutex_;
};


} // namespace Logging	
} // namespace Assist


#endif // !defined(CP_PRIORITY_LOG_DOT_H)
