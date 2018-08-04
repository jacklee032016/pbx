#if !defined(ASSIST_TPKT_CLIENT_SOCKET_HXX)
#define ASSIST_TPKT_CLIENT_SOCKET_HXX
/*
* $Id: TPKTClientSocket.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

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


/** TPKT is an extension of the TCPClientSocket that allows for
 *  packetizing the streamed information.<br><br>
 *
 *  Every packet send via the TPKT is prefixed with a header that first 
 *  indicates it is a TPKT, and that second indicates the length of the packet.
 *
 *  @see    Assist::Transport::TCPClientSocket
 *  @see    Assist::Transport::StreamClientSocket
 *  @see    Assist::Transport::Socket
 *  @see    Assist::Transport::IPAddress
 *  @see    Assist::Transport::TransportAddress
 *  @see    Assist::IO::file_descriptor_t
 *  @see    Assist::SystemException
 *  @see    Assist::SystemStatus
 */
class TPKTClientSocket : public Assist::Transport::TCPClientSocket
{

    public:

    	/** Default constructor. Create a socket whose local
    	 *  address will be a default IP address. 
	 */
    	TPKTClientSocket(const char * name = 0)
    	    throw ( Assist::SystemException );


    	/** TransportAddress constructor. Create a socket whose
    	 *  local address is bound to the given local address.
	 */				
    	TPKTClientSocket(
	    const IPAddress 	&   localAddr,
    	    const char      	*   name = 0
	)
    	    throw ( Assist::SystemException );

    	/** Connection constructor. Create a socket whose local address
    	 *  is bound to the local address given, and is connected to
    	 *  the remote address given.
	 */
    	TPKTClientSocket(
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
    	TPKTClientSocket(
	    IO::file_descriptor_t 	fd,
    	    const IPAddress 	    &   remoteAddr,
    	    const char      	    *   name = 0
	)
    	    throw ( Assist::SystemException, Assist::SystemStatus );


    	/** Virtual destructor
	 */        					
	virtual     	~TPKTClientSocket();


    	/** Text versions of a connection oriented send. 
    	 *  See StreamClientSocket::send(). This prefixes
    	 *  outgoing messages with the TPKT header.
	 */
    	virtual int 	    send(const string &)
		    	    throw ( Assist::SystemException,
				    Assist::SystemStatus,
				    Assist::Transport::ConnectionBrokenException );


    	/** Text versions of a connection oriented send. 
    	 *  See StreamClientSocket::send(). This prefixes
    	 *  outgoing messages with the TPKT header.
	 */
    	virtual int 	    send(const char *)
		    	    throw ( Assist::SystemException,
				    Assist::SystemStatus,
				    Assist::Transport::ConnectionBrokenException );


    	/** Binary version of a connection oriented send. 
    	 *  See send(const string &)
	 */
    	virtual int 	    send(const vector<u_int8_t> &)
		    	    throw ( Assist::SystemException,
				    Assist::SystemStatus,
				    Assist::Transport::ConnectionBrokenException );


    	/** Binary version of a connection oriented send. 
    	 *  See send(const string &)
	 */
    	virtual int 	    send(const u_int8_t *, size_t)
		    	    throw ( Assist::SystemException,
				    Assist::SystemStatus,
				    Assist::Transport::ConnectionBrokenException );
				

    	/** Will parse the TPKT header. If the entire TPKT header is not 
    	 *  fully received (if the underlying file descriptor is set
    	 *  nonblocking), a SystemStatus will be raised, with the status 
    	 *  set to EAGAIN. The state for the partially parsed header will 
    	 *  be saved unless clearTPKTHeader() is called. When the socket
    	 *  is again ready for reading, call this function again.
    	 *  Upon successful parsing of the header the returned size
    	 *  will be the size of the TPKT packet remaining. If the size
    	 *  is zero, either zero was sent, or the header wasn't correct.
	 */
    	u_int16_t   receiveTPKTHeader()
		    	    throw ( Assist::SystemException,
				    Assist::SystemStatus,
				    Assist::Transport::ConnectionBrokenException );

    	/** Clear the stored TPKT header information.
	 */
    	void	    clearTPKTHeader();
		

    private:
    	void	    sendTPKTHeader(u_int16_t)
		    	    throw ( Assist::SystemException,
				    Assist::SystemStatus,
				    Assist::Transport::ConnectionBrokenException );
		
    	static	const u_int8_t      TPKT_FLAG_[2];
	
    	    	u_int8_t    	    header_[4];
		int	    	    headerPosition_;
};


} // namespace Transport
} // namespace Assist


#endif // !defined(ASSIST_TPKT_CLIENT_SOCKET_HXX)
