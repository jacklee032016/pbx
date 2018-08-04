#if !defined(TPKT_SERVER_SOCKET_DOT_H)
#define TPKT_SERVER_SOCKET_DOT_H
/*
* $Id: TPKTServerSocket.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "StreamServerSocket.hxx"
#include "Sptr.hxx"
#include "TPKTClientSocket.hxx"
#include <string>


/** Infrastructure common to ASSIST.
 */
namespace Assist 
{


/** Infrastructure in ASSIST related to making network transport layer 
 *  connections.<br><br>
 */
namespace Transport 
{


/** Provides a TPKT server socket that will listen for incoming connections
 *  and create TPKTClientSockets.<br><br>
 *
 *  @see    Assist::Transport::TPKTClientSocket
 *  @see    Assist::Transport::StreamServerSocket
 *  @see    Assist::Transport::Socket
 *  @see    Assist::Transport::IPAddress
 *  @see    Assist::Transport::TransportAddress
 *  @see    Assist::SystemException
 *  @see    Assist::SystemStatus
 *  @see    Sptr
 */
class TPKTServerSocket : public Assist::Transport::StreamServerSocket
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
    	    throw ( Assist::SystemException );

    	/** TransportAddress constructor. Create a socket whose
    	 *  local address is bound to the given local address.
	 */				
    	TPKTServerSocket(
	    const IPAddress & 	localAddr,
	    const char      * 	name = 0,
	    const char      * 	acceptedName = 0,
	    int     	    	backlog = 5
	)
    	    throw ( Assist::SystemException );
	    

    	/** Virtual destructor.
	 */
	virtual     	~TPKTServerSocket()
    	    throw ( Assist::SystemException );


    	/** Accept an incoming connection, creating a TPKTClientSocket.
    	 */
    	virtual Sptr<TPKTClientSocket>  accept()
    	  	    	    	    	throw ( Assist::SystemException,
					    	Assist::SystemStatus );

    protected:
    	    	string	    	    	acceptedName_;					 
};


} // namespace Transport
} // namespace Assist


#endif // !defined(TPKT_SERVER_SOCKET_DOT_H)
