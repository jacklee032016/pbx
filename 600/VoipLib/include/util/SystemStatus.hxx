#if !defined(SYSTEM_STATUS_DOT_H)
#define SYSTEM_STATUS_DOT_H
/*
 * $Id: SystemStatus.hxx,v 1.2 2007/03/01 20:04:29 lizhijie Exp $
 */

#include "VException.hxx"


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
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


} // namespace Vocal


#endif // !defined(SYSTEM_STATUS_DOT_H)
