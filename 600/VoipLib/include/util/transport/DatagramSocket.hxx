#if !defined(VOCAL_DATAGRAM_SOCKET_HXX)
#define VOCAL_DATAGRAM_SOCKET_HXX
/*
 * $Id: DatagramSocket.hxx,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */


#include "TransportAddress.hxx"
#include "Socket.hxx"
#include "Sptr.hxx"
#include "SystemException.hxx"
#include "SystemStatus.hxx"
#include <string>
#include <vector>


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Infrastructure in VOCAL related to making network transport layer 
 *  connections.<br><br>
 */
namespace Transport 
{


/** Datagram socket is a connection-oriented or connectionless
 *  abstraction for unreliable message transport.<br><br>
 *
 *  @see    Vocal::Transport::UDPSocket
 *  @see    Vocal::Transport::Socket
 *  @see    Vocal::Transport::TransportAddress
 *  @see    Vocal::Transport::AddressFamily
 *  @see    Vocal::Transport::SocketType
 *  @see    Vocal::SystemException
 *  @see    Vocal::SystemStatus
 */
class DatagramSocket : public Vocal::Transport::Socket
{

    protected:


    	/** Construct given the addressFamily and the name.
	 */
    	DatagramSocket(const AddressFamily & addressFamily,
		       const char * name = 0)
    	    	throw ( Vocal::SystemException );


    	/** Construct given the local transport address and name.
	 */
    	DatagramSocket(const TransportAddress & localAddr,
		       const char * name = 0)
    	    	throw ( Vocal::SystemException );


    	/** Construct given the local transport address, the remote address
	 *  and name. Binds to the local address and connects to the remote 
	 *  address.
	 */
    	DatagramSocket(const TransportAddress & localAddr,
		       const TransportAddress & remoteAddr,
		       const char * name = 0)
    	    	throw ( Vocal::SystemException );


    public:    					


    	/** Virtual destructor
	 */
	virtual ~DatagramSocket();


    	/** To create a connection oriented circuit.
	 *  If connect() is used, send() and receive()
	 *  will interact with the specified remote address.
	 */
    	void	connect(const TransportAddress & remoteAddr)
		    	throw ( Vocal::SystemException );


    	/** To dissolve the connect oriented circuit.
	 */
    	void	disconnect()
			throw ( Vocal::SystemException );


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
		    throw ( Vocal::SystemException, Vocal::SystemStatus );


    	/** See send(const string &).
	 */
    	int 	send(const char *)
		    throw ( Vocal::SystemException, Vocal::SystemStatus );


    	/** Binary version of a connection oriented send. 
	 *  See send(const string &)
	 */
    	int 	send(const vector<u_int8_t> &)
		    throw ( Vocal::SystemException, Vocal::SystemStatus );


    	/** Binary version of a connection oriented send. 
	 *  See send(const string &)
	 */
    	int 	send(const u_int8_t *, size_t)
		    throw ( Vocal::SystemException, Vocal::SystemStatus );
				
    	/** Text version of a connectionless send. 
	 *  This will transmit the string to the remote address.
	 *  See send(const string &) for return semantics.
	 */
    	int 	sendTo(
		    const string &, 
		    const TransportAddress & remoteAddress
		)
		    throw ( Vocal::SystemException, Vocal::SystemStatus );


    	/** Text version of a connectionless send. 
	 *  This will transmit the string to the remote address.
	 *  See send(const string &) for return semantics.
	 */
    	int 	sendTo( 
		    const char *, 
		    const TransportAddress & remoteAddress
		)
		    throw ( Vocal::SystemException, Vocal::SystemStatus );


    	/** Binary version of a connectionless send. 
    	 *  See sendTo(const string &, const TransportAddress &).
	 */
    	int 	sendTo( 
		    const vector<u_int8_t> &,
    	    	    const TransportAddress & remoteAddress
		)
    	    	    throw ( Vocal::SystemException, Vocal::SystemStatus );


    	/** Binary version of a connectionless send. 
    	 *  See sendTo(const string &, const TransportAddress &).
	 */
    	int 	sendTo( 
	    	    const u_int8_t *, size_t,
		    const TransportAddress & remoteAddress
		)
    	    	    throw ( Vocal::SystemException, Vocal::SystemStatus );


    	/** Text version of a connection oriented receive. 
	 *  Use connect() first. This will receive the 
	 *  string from the remote address specified in connect().
	 *  Returns the number of received bytes. If a SystemStatus
	 *  was thrown either the socket was set non-blocking and it
	 *  would have blocked (status EAGAIN), or the socket was set 
	 *  blocking and it was interrupted by a signal (status EINTR). 
	 */
    	int 	receive(string &)
    	    	    throw ( Vocal::SystemException, Vocal::SystemStatus );


    	/** Text version of a connection oriented receive. 
	 *  See receiver(string &)
	 */
    	int 	receive(char *, size_t)
    	    	    throw ( Vocal::SystemException, Vocal::SystemStatus );


    	/** Binary version of a connection oriented receive. 
	 *  See receive(string &).
	 */
    	int 	receive(vector<u_int8_t> &)
    	    	    throw ( Vocal::SystemException, Vocal::SystemStatus );


    	/** Binary version of a connection oriented receive. 
	 *  See receive(string &).
	 */
    	int 	receive(u_int8_t *, size_t)
    	    	    throw ( Vocal::SystemException, Vocal::SystemStatus );


    	/** Text version of a connectionless receive. 
	 *  This will receive the string from the remote address.
	 *  See receive(string &) for return semantics.
	 */
    	int 	receiveFrom(
		    string &, 
		    TransportAddress & remoteAddress
		)
    	    	    throw ( Vocal::SystemException, Vocal::SystemStatus );


    	/** Text version of a connectionless receive. 
	 *  This will receive the string from the remote address.
	 *  See receive(string &) for return semantics.
	 */
    	int 	receiveFrom(
		    char *, size_t,
		    TransportAddress & remoteAddress
		)
    	    	    throw ( Vocal::SystemException, Vocal::SystemStatus );


    	/** Binary version of a connectionless receive. 
	 *  See receiveFrom(string &, Sptr<TransportAddress> &).
	 */
    	int 	receiveFrom( 
		    vector<u_int8_t> &,
		    TransportAddress & remoteAddress
		)
    	    	    throw ( Vocal::SystemException, Vocal::SystemStatus );


    	/** Binary version of a connectionless receive. 
	 *  See receiveFrom(string &, Sptr<TransportAddress> &).
	 */
    	int 	receiveFrom( 
		    u_int8_t *, size_t,
		    TransportAddress & remoteAddress
		)
    	    	    throw ( Vocal::SystemException, Vocal::SystemStatus );


    	/** If connected, this will return a pointer to the 
	 *  remote address. Otherwise 0 will be returned.
	 */
    	Sptr<TransportAddress>	    	getRemoteAddress() const;


    	/** Write the DatagramSocket to an ostream.
	 */
	virtual ostream &           	writeTo(ostream &) const;


    private:

    	int 	sendMessage(void *, int length, const TransportAddress *)
    	    	    throw ( Vocal::SystemException, Vocal::SystemStatus );
				
    	int 	recvMessage(void *, int length, TransportAddress *)
    	    	    throw ( Vocal::SystemException, Vocal::SystemStatus );


    	    	Sptr<TransportAddress>	    remoteAddr_;
		bool	    	    	    localAddrUpdated_;
};


} // namespace Transport
} // namespace Vocal


#endif // !defined(VOCAL_DATAGRAM_SOCKET_HXX)
