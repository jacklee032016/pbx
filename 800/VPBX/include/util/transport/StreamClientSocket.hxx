#if !defined(ASSIST_STREAM_CLIENT_SOCKET_HXX)
#define ASSIST_STREAM_CLIENT_SOCKET_HXX
/*
* $Id: StreamClientSocket.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "Socket.hxx"
#include "Sptr.hxx"
#include "TransportAddress.hxx"
#include "SystemException.hxx"
#include "SystemStatus.hxx"
#include "ConnectionBrokenException.hxx"
#include <string>
#include <vector>


/** Infrastructure common to ASSIST.
 */
namespace Assist 
{


/** Infrastructure in ASSIST related to making network transport layer 
 *  connections.<br><br>
 */
namespace Transport 
{


/** Stream client socket is a connection-oriented abstraction for reliable 
 *  message transport.<br><br>
 *
 *  @see    Assist::Transport::Socket
 *  @see    Assist::Transport::TransportAddress
 *  @see    Assist::Transport::AddressFamily
 *  @see    Assist::IO::file_descriptor_t
 *  @see    Assist::SystemException
 *  @see    Assist::SystemStatus
 *  @see    Assist::Transport::ConnectionBrokenException
 */
class StreamClientSocket : public Assist::Transport::Socket
{

    public:


    	/** Address family constructor. Create a socket whose local
	 *  address will be a default transport address corresponding 
	 *  to the given address family. This usually produces an 
	 *  "ANY" address that will be allocated by the operating 
	 *  system.
	 */
    	StreamClientSocket(
	    const AddressFamily     & 	addressFamily,
    	    const char      	    * 	name = 0
	)
    	    throw ( Assist::SystemException );


    	/** TransportAddress constructor. Create a socket whose
	 *  local address is bound to the given local address.
	 */				
    	StreamClientSocket(
	    const TransportAddress  &   localAddr,
    	    const char      	    *   name = 0
	)
    	    throw ( Assist::SystemException );


    	/** Connection constructor. Create a socket whose local address
	 *  is bound to the local address given, and is connected to
	 *  the remote address given.
	 */
    	StreamClientSocket(
    	    const TransportAddress  &   localAddr,
	    const TransportAddress  &   remoteAddr,
	    const char      	    *   name = 0
	)
    	    throw ( Assist::SystemException, Assist::SystemStatus );


    	/** Accepted connection constructor. Create a socket from an
	 *  already established socket, i.e. via accept. This socket
	 *  is connected to the remote address given. If the file
	 *  descriptor passed in is invalid, a new socket will
	 *  be created, bound to the an "ANY" address and connected
	 *  to the remote address.
	 */
    	StreamClientSocket(
	    IO::file_descriptor_t    	fd, 
    	    const TransportAddress  &   remoteAddr,
	    const char      	    *   name = 0
	)
    	    throw ( Assist::SystemException, Assist::SystemStatus );
    					

    	/** Virtual destructor
	 */
	virtual     	~StreamClientSocket();


    	/** To create a connection oriented circuit.
	 *  A nonblocking socket may issue an SystemStatus
	 *  exception with a status value of EINPROGRESS. It is
	 *  then possible to select for completion of the connection, 
	 *  i.e via Selector::Select, the socket for writing.
	 */
    	void	connect(const TransportAddress & remoteAddr)
		    throw ( Assist::SystemException, Assist::SystemStatus );


    	/** To complete the connection when the nonblocking has 
	 *  been connect()-ed and the socket has return
	 *  an EINPROGRESS status.
	 */
    	void	completeConnection()
	    	    throw ( Assist::SystemException );
				

    	/** Text versions of a connection oriented send. 
	 *  Use connect() first. This will transmit the 
	 *  string to the remote address specified in connect().
	 *  Returns the number of bytes sent. If 0 was returned, there
	 *  was an empty message to send, and nothing was sent. If a 
	 *  SystemStatus was thrown the socket was set non-blocking 
	 *  and it would have blocked (an EAGAIN status), or the 
	 *  socket was set blocking and it was interrupted by a signal
	 *  (an EINTR) signal. 
	 */
    	virtual int 	send(const string &)
		    	throw ( Assist::SystemException,
				Assist::SystemStatus,
				Assist::Transport::ConnectionBrokenException );

    	/** Text versions of a connection oriented send. 
	 *  See send(const string &).
	  */
    	virtual int 	send(const char *)
		    	throw ( Assist::SystemException,
				Assist::SystemStatus,
				Assist::Transport::ConnectionBrokenException );


    	/** Binary version of a connection oriented send. 
	 *  See send(const string &)
	 */
    	virtual int 	send(const vector<u_int8_t> &)
		    	throw ( Assist::SystemException,
				Assist::SystemStatus,
				Assist::Transport::ConnectionBrokenException );


    	/** Binary version of a connection oriented send. 
	 *  See send(const string &)
	 */
    	virtual int 	send(const u_int8_t *, size_t)
		    	throw ( Assist::SystemException,
				Assist::SystemStatus,
				Assist::Transport::ConnectionBrokenException );
				

    	/** Text version of a connection oriented receive. 
	 *  Use connect() first. This will receive the 
	 *  string from the remote address specified in connect().
	 *  Returns the number of received bytes. If a SystemStatus
	 *  was thrown either the socket was set non-blocking and it
	 *  would have blocked (status EAGAIN), or the socket was set blocking
	 *  and it was interrupted by a signal (status EINTR). 
	 */
    	virtual int 	receive(string &)
		    	throw ( Assist::SystemException,
				Assist::SystemStatus,
				Assist::Transport::ConnectionBrokenException );


    	/** Text version of a connection oriented receive. 
	 *  See receive(string &).
	 */
    	virtual int 	receive(char *, size_t)
		    	throw ( Assist::SystemException,
				Assist::SystemStatus,
				Assist::Transport::ConnectionBrokenException );


    	/** Binary version of a connection oriented receive. 
	 *  See receive(string &).
	 */
    	virtual int 	receive(vector<u_int8_t> &)
		    	throw ( Assist::SystemException,
				Assist::SystemStatus,
				Assist::Transport::ConnectionBrokenException );
				

    	/** Binary version of a connection oriented receive. 
	 *  See receive(string &).
	 */
    	virtual int 	receive(u_int8_t *, size_t)
		    	throw ( Assist::SystemException,
				Assist::SystemStatus,
				Assist::Transport::ConnectionBrokenException );


    	/** If connected, this will return a pointer to the 
	 *  remote address. Otherwise 0 will be returned.
	 */
    	Sptr<TransportAddress>	getRemoteAddress() const;


    	/** Write the socket to an ostream.
	 */
	virtual ostream &   	writeTo(ostream &) const;
		

    private:

    	int 	sendMessage(void *, int)
		    	throw ( Assist::SystemException,
				Assist::SystemStatus,
				Assist::Transport::ConnectionBrokenException );

    	int 	recvMessage(void *, int)
		    	throw ( Assist::SystemException,
				Assist::SystemStatus,
				Assist::Transport::ConnectionBrokenException );


    	Sptr<TransportAddress>	    remoteAddr_;
	bool	    	    	    localAddrUpdated_;
	bool	    	    	    connectionCompleted_;
};


} // namespace Transport
} // namespace Assist


#endif // !defined(ASSIST_STREAM_CLIENT_SOCKET_HXX)
