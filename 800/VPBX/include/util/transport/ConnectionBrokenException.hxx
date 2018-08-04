#if !defined(CONNECTION_BROKEN_EXCEPTION_DOT_H)
#define CONNECTION_BROKEN_EXCEPTION_DOT_H
/*
* $Id: ConnectionBrokenException.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "SystemException.hxx"


/** Infrastructure common to ASSIST.
 */
namespace Assist 
{


/** Infrastructure in ASSIST related to making network transport layer 
 *  connections.<br><br>
 */
namespace Transport 
{



/** Exception thrown when a TCP connection is broken.<br><br>
 *
 *  @see    Assist::SystemException
 */
class ConnectionBrokenException : public Assist::SystemException
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
        ConnectionBrokenException(  
	    const string	    & 	msg,
    	    const string    	    & 	file,
    	    const int 	    	    	line,
    	    const int 	    	    	error = 0,
	    bool    	    	    	warning = true);
    

    	/** Name of this class of exception, "ConnectionBrokenException".
    	 */
    	string      	    	    getName() const;
};


} // namespace Transport
} // namespace Assist


#endif // !defined(CONNECTION_BROKEN_EXCEPTION_DOT_H)
