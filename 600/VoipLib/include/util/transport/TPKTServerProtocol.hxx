#if !defined(TPKT_SERVER_PROTOCOL_DOT_H)
#define TPKT_SERVER_PROTOCOL_DOT_H
/*
 * $Id: TPKTServerProtocol.hxx,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */


#include "Protocol.hxx"
#include "Sptr.hxx"
#include "TPKTClientSocket.hxx"


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Infrastructure in VOCAL related to making network transport layer 
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
 *  @see    Vocal::Transport::Protocol
 *  @see    Vocal::Transport::TPKTServerSocket
 *  @see    Vocal::SystemException
 *  @see    Vocal::SystemStatus
 *  @see    Sptr
 */
class TPKTServerProtocol : public Vocal::Transport::Protocol
{

    public:


    	/** Create the protocol with an optional name.
	 */
    	TPKTServerProtocol(const char * name);
	
	
    	/** Virtual destructor.
	 */
    	virtual ~TPKTServerProtocol()
	    throw ( Vocal::SystemException );



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
		throw ( Vocal::SystemException, Vocal::SystemStatus );
};


} // namespace Transport
} // namespace Vocal


#endif // !defined(TPKT_SERVER_PROTOCOL_DOT_H)
