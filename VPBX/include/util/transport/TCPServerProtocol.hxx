#if !defined(ASSIST_TCP_SERVER_PROTOCOL_HXX)
#define ASSIST_TCP_SERVER_PROTOCOL_HXX
/*
* $Id: TCPServerProtocol.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "Protocol.hxx"
#include "Sptr.hxx"
#include "TCPClientSocket.hxx"


/** Infrastructure common to ASSIST.
 */
namespace Assist 
{


/** Infrastructure in ASSIST related to making network transport layer 
 *  connections.<br><br>
 */
namespace Transport 
{


class TCPServerSocket;


/** Since there are no bytes flowing in a connection with a server socket,
 *  the meaning associated with incoming availability on the file descriptor
 *  is an incoming socket connection.<br><br> 
 *
 *  The TCPServerProtocol accepts this connection from the socket and 
 *  indicates to the user this via the onAcceptedConnection callback.
 *
 *  @see    Assist::Transport::Protocol
 *  @see    Assist::Transport::TCPServerSocket
 *  @see    Assist::SystemException
 *  @see    Assist::SystemStatus
 *  @see    Sptr
 */
class TCPServerProtocol : public Assist::Transport::Protocol
{

    public:


    	/** Create the protocol with an optional name.
	 */
    	TCPServerProtocol(const char * name = 0);


    	/** Virtual destructor.
	 */
    	virtual ~TCPServerProtocol();


    	/** Get the TCPServerSocket associated with this protocol.
	 */
    	virtual TCPServerSocket &     	    
	    	getTCPServerSocket() = 0;
		

    	/** Callback to notify the user of an accepted connection.
	 */
    	virtual void	    	    	    
	    	onAcceptedConnection(Sptr<TCPClientSocket>) = 0;


    	/** Override's Protocol's onIncomingAvailable. When a server socket
	 *  has incoming available, it needs to accept the connection and
	 *  then call onAcceptedConnection.
	 */
	virtual void	    	    	    
	    	onIncomingAvailable()
		throw ( Assist::SystemException, Assist::SystemStatus );
};


} // namespace Transport
} // namespace Assist


#endif // !defined(ASSIST_TCP_SERVER_PROTOCOL_HXX)
