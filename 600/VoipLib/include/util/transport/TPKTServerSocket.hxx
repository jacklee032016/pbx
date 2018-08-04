#if !defined(TPKT_SERVER_SOCKET_DOT_H)
#define TPKT_SERVER_SOCKET_DOT_H
/*
 * $Id: TPKTServerSocket.hxx,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */


#include "StreamServerSocket.hxx"
#include "Sptr.hxx"
#include "TPKTClientSocket.hxx"
#include <string>


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Infrastructure in VOCAL related to making network transport layer 
 *  connections.<br><br>
 */
namespace Transport 
{


/** Provides a TPKT server socket that will listen for incoming connections
 *  and create TPKTClientSockets.<br><br>
 *
 *  @see    Vocal::Transport::TPKTClientSocket
 *  @see    Vocal::Transport::StreamServerSocket
 *  @see    Vocal::Transport::Socket
 *  @see    Vocal::Transport::IPAddress
 *  @see    Vocal::Transport::TransportAddress
 *  @see    Vocal::SystemException
 *  @see    Vocal::SystemStatus
 *  @see    Sptr
 */
class TPKTServerSocket : public Vocal::Transport::StreamServerSocket
{

    public:


    	/** Address family constructor. Create a server socket whose 
    	 *  local address will be a default IP address, corresponding 
    	 *  INADDR_ANY.
	 */
    	TPKTServerSocket(
	    const char      * 	name = 0,
    	    const char      * 	acceptedName = 0,
	    int     	    	backlog = 5
	)
    	    throw ( Vocal::SystemException );

    	/** TransportAddress constructor. Create a socket whose
    	 *  local address is bound to the given local address.
	 */				
    	TPKTServerSocket(
	    const IPAddress & 	localAddr,
	    const char      * 	name = 0,
	    const char      * 	acceptedName = 0,
	    int     	    	backlog = 5
	)
    	    throw ( Vocal::SystemException );
	    

    	/** Virtual destructor.
	 */
	virtual     	~TPKTServerSocket()
    	    throw ( Vocal::SystemException );


    	/** Accept an incoming connection, creating a TPKTClientSocket.
    	 */
    	virtual Sptr<TPKTClientSocket>  accept()
    	  	    	    	    	throw ( Vocal::SystemException,
					    	Vocal::SystemStatus );

    protected:
    	    	string	    	    	acceptedName_;					 
};


} // namespace Transport
} // namespace Vocal


#endif // !defined(TPKT_SERVER_SOCKET_DOT_H)
