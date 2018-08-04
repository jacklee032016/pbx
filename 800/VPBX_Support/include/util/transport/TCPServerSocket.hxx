#if !defined(ASSIST_TCP_SERVER_SOCKET_HXX)
#define ASSIST_TCP_SERVER_SOCKET_HXX
/*
* $Id: TCPServerSocket.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "StreamServerSocket.hxx"
#include "Sptr.hxx"
#include "TCPClientSocket.hxx"
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


class IPAddress;


/** Provides a TCP/IP socket that will listen for incoming connections.<br><br>
 *
 *  @see    Assist::Transport::TCPClientSocket
 *  @see    Assist::Transport::StreamServerSocket
 *  @see    Assist::Transport::Socket
 *  @see    Assist::Transport::IPAddress
 *  @see    Assist::Transport::TransportAddress
 *  @see    Assist::SystemException
 *  @see    Assist::SystemStatus
 *  @see    Sptr
 */
class TCPServerSocket : public Assist::Transport::StreamServerSocket
{

    public:


    	/** Create a server socket whose local address will be a 
	 *  default IP address, corresponding INADDR_ANY.
	 */
    	TCPServerSocket(
	    int     	    backlog = 5,
	    const char  *   name = 0,
	    const char  *   acceptedName = 0
	)
    	    throw ( Assist::SystemException );


    	/** TransportAddress constructor. Create a socket whose
    	 *  local address is bound to the given local address.
	 */				
    	TCPServerSocket(
	    const IPAddress 	    &   localAddr,
	    int     	    	    	backlog = 5,
	    const char      	    *   name = 0,
	    const char      	    *   acceptedName = 0
	)
    	    throw ( Assist::SystemException );

    	/** Virtual destructor.
	 */
	virtual     	~TCPServerSocket();


    	/** Accept an incoming connection, creating a TCPClientSocket.
    	 */
    	virtual Sptr<TCPClientSocket> 	accept()
    	  	    	    	    	throw ( Assist::SystemException,
					    	Assist::SystemStatus );

    private:
    	    	string	    	    	acceptedName_;					 
};


} // namespace Transport
} // namespace Assist


#endif // !defined(ASSIST_TCP_SERVER_SOCKET_HXX)
