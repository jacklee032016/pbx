#if !defined(ASSIST_PROTOCOL_EXCEPTION_HXX)
#define ASSIST_PROTOCOL_EXCEPTION_HXX
/*
* $Id: ProtocolException.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "VException.hxx"


/** Infrastructure common to ASSIST.
 */
namespace Assist 
{


/** Infrastructure in ASSIST related to making network transport layer 
 *  connections.<br><br>
 */
namespace Transport 
{


class Protocol;


/** Thrown when the protocol is notified of exceptional events, such
 *  as the file descriptor associated with the protocol disconnecting
 *  or when an error is detected on the associated file descriptor.<br><br>
 *
 *  @see    Assist::Transport::Protocol
 *  @see    VException
 */
class ProtocolException : public VException
{
    public:
    	
    	/** Types of exceptional conditions.
	 */
	enum Type
	{
	    NO_EXCEPTION = 0,
	    HANGUP_EXCEPTION,
	    ERROR_EXCEPTION
	};
	

    	/** constructor
	 */
        ProtocolException(  
	    const string	    & 	msg,
    	    Type    	    	    	type,
	    Protocol	    	    &	protocol);
    
    	
	/** Get class of exception name, "ProtocolException".
	 */
    	string      	    	    getName() const;


    	/** Accessor
	 */
    	Type	    	    	    getType() const;
	

	/** Access the protocol associated with the exceptional condition.
	 */
    	Protocol &  	    	    getProtocol() const;
	

    private:

    	Type	    	    	    	myType;
    	Protocol    	    	    &	myProtocol;
};


} // namespace Transport
} // namespace Assist


#endif // !defined(ASSIST_PROTOCOL_EXCEPTION_HXX)
