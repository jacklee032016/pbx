#if !defined(VOCAL_TCP_SERVER_PROTOCOL_HXX)
#define VOCAL_TCP_SERVER_PROTOCOL_HXX
/*
 * $Id: TCPServerProtocol.hxx,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */


#include "Protocol.hxx"
#include "Sptr.hxx"
#include "TCPClientSocket.hxx"


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Infrastructure in VOCAL related to making network transport layer 
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
 *  @see    Vocal::Transport::Protocol
 *  @see    Vocal::Transport::TCPServerSocket
 *  @see    Vocal::SystemException
 *  @see    Vocal::SystemStatus
 *  @see    Sptr
 */
class TCPServerProtocol : public Vocal::Transport::Protocol
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
		throw ( Vocal::SystemException, Vocal::SystemStatus );
};


} // namespace Transport
} // namespace Vocal


#endif // !defined(VOCAL_TCP_SERVER_PROTOCOL_HXX)
