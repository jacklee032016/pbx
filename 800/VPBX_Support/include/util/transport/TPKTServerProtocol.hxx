#if !defined(TPKT_SERVER_PROTOCOL_DOT_H)
#define TPKT_SERVER_PROTOCOL_DOT_H
/*
* $Id: TPKTServerProtocol.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "Protocol.hxx"
#include "Sptr.hxx"
#include "TPKTClientSocket.hxx"


/** Infrastructure common to ASSIST.
 */
namespace Assist 
{


/** Infrastructure in ASSIST related to making network transport layer 
 *  connections.<br><br>
 */
namespace Transport 
{


class TPKTServerSocket;


/** Since there are no bytes flowing in a connection with a server socket,
 *  the meaning associated with incoming availability on the file descriptor
 *  is an incoming socket connection.<br><br>
 *
 *  The TPKTServerProtocol accepts this connection from the socket and 
 *  indicates to the user this via the onAcceptedConnection callback.
 *
 *  @see    Assist::Transport::Protocol
 *  @see    Assist::Transport::TPKTServerSocket
 *  @see    Assist::SystemException
 *  @see    Assist::SystemStatus
 *  @see    Sptr
 */
class TPKTServerProtocol : public Assist::Transport::Protocol
{

    public:


    	/** Create the protocol with an optional name.
	 */
    	TPKTServerProtocol(const char * name);
	
	
    	/** Virtual destructor.
	 */
    	virtual ~TPKTServerProtocol()
	    throw ( Assist::SystemException );



    	/** Get the TPKTServerSocket associated with this protocol.
	 */
    	virtual TPKTServerSocket &     	    
	    	getTPKTServerSocket() = 0;
		

    	/** Callback to notify the user of an accepted connection.
	 */
    	virtual void	    	    	    
	    	onAcceptedConnection(Sptr<TPKTClientSocket>) = 0;


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


#endif // !defined(TPKT_SERVER_PROTOCOL_DOT_H)
