#if !defined(VOCAL_TCP_SERVER_SOCKET_HXX)
#define VOCAL_TCP_SERVER_SOCKET_HXX
/*
 * $Id: TCPServerSocket.hxx,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */

#include "StreamServerSocket.hxx"
#include "Sptr.hxx"
#include "TCPClientSocket.hxx"
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


class IPAddress;


/** Provides a TCP/IP socket that will listen for incoming connections.<br><br>
 *
 *  @see    Vocal::Transport::TCPClientSocket
 *  @see    Vocal::Transport::StreamServerSocket
 *  @see    Vocal::Transport::Socket
 *  @see    Vocal::Transport::IPAddress
 *  @see    Vocal::Transport::TransportAddress
 *  @see    Vocal::SystemException
 *  @see    Vocal::SystemStatus
 *  @see    Sptr
 */
class TCPServerSocket : public Vocal::Transport::StreamServerSocket
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
    	    throw ( Vocal::SystemException );


    	/** TransportAddress constructor. Create a socket whose
    	 *  local address is bound to the given local address.
	 */				
    	TCPServerSocket(
	    const IPAddress 	    &   localAddr,
	    int     	    	    	backlog = 5,
	    const char      	    *   name = 0,
	    const char      	    *   acceptedName = 0
	)
    	    throw ( Vocal::SystemException );

    	/** Virtual destructor.
	 */
	virtual     	~TCPServerSocket();


    	/** Accept an incoming connection, creating a TCPClientSocket.
    	 */
    	virtual Sptr<TCPClientSocket> 	accept()
    	  	    	    	    	throw ( Vocal::SystemException,
					    	Vocal::SystemStatus );

    private:
    	    	string	    	    	acceptedName_;					 
};


} // namespace Transport
} // namespace Vocal


#endif // !defined(VOCAL_TCP_SERVER_SOCKET_HXX)
