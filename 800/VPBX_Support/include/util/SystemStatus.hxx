#if !defined(SYSTEM_STATUS_DOT_H)
#define SYSTEM_STATUS_DOT_H
/*
* $Id: SystemStatus.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "VException.hxx"


/** Infrastructure common to ASSIST.
 */
namespace Assist 
{


/** Exception thrown when an unusual, but non-error, condition
 *  happens.<br><br><br>
 *
 *  This other is usually used with EAGAIN.
 */
class SystemStatus : public VException
{
    public:
    

    	/** Create the status.
	 *
	 *  @param  msg     Message to be logged. It's usually a strerror.
	 *  @param  file    File name where exception was created.
	 *  @param  line    Line number of the file where the exception was 
	 *  	    	    created.
	 *  @param  error   Usually an errno, and usually EAGAIN.
	 */
        SystemStatus(  
	    const string	    & 	msg,
    	    const string    	    & 	file,
    	    const int 	    	    	line,
    	    const int 	    	    	status = 0);
    

    	/** Name of this class of exception, "SystemStatus".
    	 */
    	string      	    	    getName() const;


    	/** Returns the error code, usually an errno, usually EAGAIN.
	 */
    	int   	    	    	    getStatus() const;
};


} // namespace Assist


#endif // !defined(SYSTEM_STATUS_DOT_H)
