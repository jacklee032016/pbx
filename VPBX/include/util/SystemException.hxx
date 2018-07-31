#if !defined(SYSTEM_EXCEPTION_DOT_H)
#define SYSTEM_EXCEPTION_DOT_H
/*
* $Id: SystemException.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "VException.hxx"


/** Infrastructure common to ASSIST.
 */
namespace Assist 
{


/** Exception caused by a system error.<br><br>
 */
class SystemException: public VException
{
    public:
    
    	/** Create the exception.
	 *
	 *  @param  msg     Message to be logged. It's usually a strerror.
	 *  @param  file    File name where exception was created.
	 *  @param  line    Line number of the file where the exception was 
	 *  	    	    created.
	 *  @param  error   Usually an errno.
	 *  @param  warn    If set, will cause a LOG_WARN to be generated.
	 */
        SystemException(  
	    const string	    & 	msg,
    	    const string    	    & 	file,
    	    const int 	    	    	line,
    	    const int 	    	    	error = 0,
	    bool    	    	    	warning = true);
    

    	/** Name of this class of exception, "SystemException".
    	 */
    	string      	    	    getName() const;


    	/** Returns the error code, usually an errno.
	 */
    	int   	    	    	    getError() const;
};


} // namespace Assist


#endif // !defined(SYSTEM_EXCEPTION_DOT_H)
