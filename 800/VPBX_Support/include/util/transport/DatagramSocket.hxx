#if !defined(ASSIST_DATAGRAM_SOCKET_HXX)
#define ASSIST_DATAGRAM_SOCKET_HXX
/*
* $Id: DatagramSocket.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "TransportAddress.hxx"
#include "Socket.hxx"
#include "Sptr.hxx"
#include "SystemException.hxx"
#include "SystemStatus.hxx"
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


/** Datagram socket is a connection-oriented or connectionless
 *  abstraction for unreliable message transport.<br><br>
 *
 *  @see    Assist::Transport::UDPSocket
 *  @see    Assist::Transport::Socket
 *  @see    Assist::Transport::TransportAddress
 *  @see    Assist::Transport::AddressFamily
 *  @see    Assist::Transport::SocketType
 *  @see    Assist::SystemException
 *  @see    Assist::SystemStatus
 */
class DatagramSocket : public Assist::Transport::Socket
{

    protected:


    	/** Construct given the addressFamily and the name.
	 */
    	DatagramSocket(const AddressFamily & addressFamily,
		       const char * name = 0)
    	    	throw ( Assist::SystemException );


    	/** Construct given the local transport address and name.
	 */
    	DatagramSocket(const TransportAddress & localAddr,
		       const char * name = 0)
    	    	throw ( Assist::SystemException );


    	/** Construct given the local transport address, the remote address
	 *  and name. Binds to the local address and connects to the remote 
	 *  address.
	 */
    	DatagramSocket(const TransportAddress & localAddr,
		       const TransportAddress & remoteAddr,
		       const char * name = 0)
    	    	throw ( Assist::SystemException );


    public:    					


    	/** Virtual destructor
	 */
	virtual ~DatagramSocket();


    	/** To create a connection oriented circuit.
	 *  If connect() is used, send() and receive()
	 *  will interact with the specified remote address.
	 */
    	void	connect(const TransportAddress & remoteAddr)
		    	throw ( Assist::SystemException );


    	/** To dissolve the connect oriented circuit.
	 */
    	void	disconnect()
			throw ( Assist::SystemException );


    	/** Text version of a connection oriented send. 
	 *  Use connect() first. This will transmit the 
	 *  string to the remote address specified in connect().
	 *  Returns the number of bytes sent. If 0 was returned, 
	 *  there either was an empty message to send. If 
	 *  a SystemStatus was thrown, the socket was set 
	 *  non-blocking and it would have blocked (status EAGAIN), 
	 *  or the socket was set blocking and it was interrupted 
	 *  by a signal (status EINTR). 
	 */
    	int 	send(const string &)
		    throw ( Assist::SystemException, Assist::SystemStatus );


    	/** See send(const string &).
	 */
    	int 	send(const char *)
		    throw ( Assist::SystemException, Assist::SystemStatus );


    	/** Binary version of a connection oriented send. 
	 *  See send(const string &)
	 */
    	int 	send(const vector<u_int8_t> &)
		    throw ( Assist::SystemException, Assist::SystemStatus );


    	/** Binary version of a connection oriented send. 
	 *  See send(const string &)
	 */
    	int 	send(const u_int8_t *, size_t)
		    throw ( Assist::SystemException, Assist::SystemStatus );
				
    	/** Text version of a connectionless send. 
	 *  This will transmit the string to the remote address.
	 *  See send(const string &) for return semantics.
	 */
    	int 	sendTo(
		    const string &, 
		    const TransportAddress & remoteAddress
		)
		    throw ( Assist::SystemException, Assist::SystemStatus );


    	/** Text version of a connectionless send. 
	 *  This will transmit the string to the remote address.
	 *  See send(const string &) for return semantics.
	 */
    	int 	sendTo( 
		    const char *, 
		    const TransportAddress & remoteAddress
		)
		    throw ( Assist::SystemException, Assist::SystemStatus );


    	/** Binary version of a connectionless send. 
    	 *  See sendTo(const string &, const TransportAddress &).
	 */
    	int 	sendTo( 
		    const vector<u_int8_t> &,
    	    	    const TransportAddress & remoteAddress
		)
    	    	    throw ( Assist::SystemException, Assist::SystemStatus );


    	/** Binary version of a connectionless send. 
    	 *  See sendTo(const string &, const TransportAddress &).
	 */
    	int 	sendTo( 
	    	    const u_int8_t *, size_t,
		    const TransportAddress & remoteAddress
		)
    	    	    throw ( Assist::SystemException, Assist::SystemStatus );


    	/** Text version of a connection oriented receive. 
	 *  Use connect() first. This will receive the 
	 *  string from the remote address specified in connect().
	 *  Returns the number of received bytes. If a SystemStatus
	 *  was thrown either the socket was set non-blocking and it
	 *  would have blocked (status EAGAIN), or the socket was set 
	 *  blocking and it was interrupted by a signal (status EINTR). 
	 */
    	int 	receive(string &)
    	    	    throw ( Assist::SystemException, Assist::SystemStatus );


    	/** Text version of a connection oriented receive. 
	 *  See receiver(string &)
	 */
    	int 	receive(char *, size_t)
    	    	    throw ( Assist::SystemException, Assist::SystemStatus );


    	/** Binary version of a connection oriented receive. 
	 *  See receive(string &).
	 */
    	int 	receive(vector<u_int8_t> &)
    	    	    throw ( Assist::SystemException, Assist::SystemStatus );


    	/** Binary version of a connection oriented receive. 
	 *  See receive(string &).
	 */
    	int 	receive(u_int8_t *, size_t)
    	    	    throw ( Assist::SystemException, Assist::SystemStatus );


    	/** Text version of a connectionless receive. 
	 *  This will receive the string from the remote address.
	 *  See receive(string &) for return semantics.
	 */
    	int 	receiveFrom(
		    string &, 
		    TransportAddress & remoteAddress
		)
    	    	    throw ( Assist::SystemException, Assist::SystemStatus );


    	/** Text version of a connectionless receive. 
	 *  This will receive the string from the remote address.
	 *  See receive(string &) for return semantics.
	 */
    	int 	receiveFrom(
		    char *, size_t,
		    TransportAddress & remoteAddress
		)
    	    	    throw ( Assist::SystemException, Assist::SystemStatus );


    	/** Binary version of a connectionless receive. 
	 *  See receiveFrom(string &, Sptr<TransportAddress> &).
	 */
    	int 	receiveFrom( 
		    vector<u_int8_t> &,
		    TransportAddress & remoteAddress
		)
    	    	    throw ( Assist::SystemException, Assist::SystemStatus );


    	/** Binary version of a connectionless receive. 
	 *  See receiveFrom(string &, Sptr<TransportAddress> &).
	 */
    	int 	receiveFrom( 
		    u_int8_t *, size_t,
		    TransportAddress & remoteAddress
		)
    	    	    throw ( Assist::SystemException, Assist::SystemStatus );


    	/** If connected, this will return a pointer to the 
	 *  remote address. Otherwise 0 will be returned.
	 */
    	Sptr<TransportAddress>	    	getRemoteAddress() const;


    	/** Write the DatagramSocket to an ostream.
	 */
	virtual ostream &           	writeTo(ostream &) const;


    private:

    	int 	sendMessage(void *, int length, const TransportAddress *)
    	    	    throw ( Assist::SystemException, Assist::SystemStatus );
				
    	int 	recvMessage(void *, int length, TransportAddress *)
    	    	    throw ( Assist::SystemException, Assist::SystemStatus );


    	    	Sptr<TransportAddress>	    remoteAddr_;
		bool	    	    	    localAddrUpdated_;
};


} // namespace Transport
} // namespace Assist


#endif // !defined(ASSIST_DATAGRAM_SOCKET_HXX)
