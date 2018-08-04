#if !defined(VOCAL_TCP_CLIENT_SOCKET_HXX)
#define VOCAL_TCP_CLIENT_SOCKET_HXX
/*
 * $Id: TCPClientSocket.hxx,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */


#include "StreamClientSocket.hxx"
#include "IPAddress.hxx"


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


/** TCP client socket is a connection-oriented abstraction for 
 *  client transport mechanisms using the reliable message transport
 *  protocol TCP/IP.<br><br>
 *
 *  @see    Vocal::Transport::TPKTClientSocket
 *  @see    Vocal::Transport::StreamClientSocket
 *  @see    Vocal::Transport::Socket
 *  @see    Vocal::Transport::IPAddress
 *  @see    Vocal::Transport::TransportAddress
 *  @see    Vocal::IO::file_descriptor_t
 *  @see    Vocal::SystemException
 *  @see    Vocal::SystemStatus
 */
class TCPClientSocket : public Vocal::Transport::StreamClientSocket
{
    public:


    	/** Default constructor. Create a socket whose local
    	 *  address will be a default IP address. 
	 */
    	TCPClientSocket(const char * name = 0)
    	    throw ( Vocal::SystemException );


    	/** TransportAddress constructor. Create a socket whose
    	 *  local address is bound to the given local address.
    	 */
    	TCPClientSocket(
	    const IPAddress 	&   localAddr,
    	    const char      	*   name = 0
	)
    	    throw ( Vocal::SystemException );


    	/** Connection constructor. Create a socket whose local address
    	 *  is bound to the local address given, and is connected to
    	 *  the remote address given.
	 */
    	TCPClientSocket(
	    const IPAddress 	&   localAddr,
	    const IPAddress 	&   remoteAddr,
	    const char      	*   name = 0
	)
    	    throw ( Vocal::SystemException, Vocal::SystemStatus );

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
    	    throw ( Vocal::SystemException, Vocal::SystemStatus );

        					
    	/** Virtual destructor
	 */
	virtual     	~TCPClientSocket();


    	/** If connected, this will return a pointer to the remote ip address.
	 *  Otherwise 0 will be returned.
    	 */
	Sptr<IPAddress>     getRemoteIPAddress() const;
};


} // namespace Transport
} // namespace Vocal


#endif // !defined(VOCAL_TCP_CLIENT_SOCKET_HXX)
