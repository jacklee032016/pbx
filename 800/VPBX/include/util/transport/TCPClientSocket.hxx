#if !defined(ASSIST_TCP_CLIENT_SOCKET_HXX)
#define ASSIST_TCP_CLIENT_SOCKET_HXX
/*
* $Id: TCPClientSocket.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "StreamClientSocket.hxx"
#include "IPAddress.hxx"


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


/** TCP client socket is a connection-oriented abstraction for 
 *  client transport mechanisms using the reliable message transport
 *  protocol TCP/IP.<br><br>
 *
 *  @see    Assist::Transport::TPKTClientSocket
 *  @see    Assist::Transport::StreamClientSocket
 *  @see    Assist::Transport::Socket
 *  @see    Assist::Transport::IPAddress
 *  @see    Assist::Transport::TransportAddress
 *  @see    Assist::IO::file_descriptor_t
 *  @see    Assist::SystemException
 *  @see    Assist::SystemStatus
 */
class TCPClientSocket : public Assist::Transport::StreamClientSocket
{
    public:


    	/** Default constructor. Create a socket whose local
    	 *  address will be a default IP address. 
	 */
    	TCPClientSocket(const char * name = 0)
    	    throw ( Assist::SystemException );


    	/** TransportAddress constructor. Create a socket whose
    	 *  local address is bound to the given local address.
    	 */
    	TCPClientSocket(
	    const IPAddress 	&   localAddr,
    	    const char      	*   name = 0
	)
    	    throw ( Assist::SystemException );


    	/** Connection constructor. Create a socket whose local address
    	 *  is bound to the local address given, and is connected to
    	 *  the remote address given.
	 */
    	TCPClientSocket(
	    const IPAddress 	&   localAddr,
	    const IPAddress 	&   remoteAddr,
	    const char      	*   name = 0
	)
    	    throw ( Assist::SystemException, Assist::SystemStatus );

    	/** Accepted connection constructor. Create a socket from an
    	 *  already established socket, i.e. via accept. This socket
    	 *  is connected to the remote address given. If the file
    	 *  descriptor passed in is invalid, a new socket will
    	 *  be created, bound to the an INADDR_ANY and connected
    	 *  to the remote address.
	 */
    	TCPClientSocket(
	    IO::file_descriptor_t    	    fd,
    	    const IPAddress 		&   remoteAddr,
    	    const char      	    	*   name = 0
	)
    	    throw ( Assist::SystemException, Assist::SystemStatus );

        					
    	/** Virtual destructor
	 */
	virtual     	~TCPClientSocket();


    	/** If connected, this will return a pointer to the remote ip address.
	 *  Otherwise 0 will be returned.
    	 */
	Sptr<IPAddress>     getRemoteIPAddress() const;
};


} // namespace Transport
} // namespace Assist


#endif // !defined(ASSIST_TCP_CLIENT_SOCKET_HXX)
