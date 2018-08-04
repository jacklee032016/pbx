#if !defined(SOCKET_OPTIONS_DOT_H)
#define SOCKET_OPTIONS_DOT_H
/*
 * $Id: SocketOptions.hxx,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */


#include "SystemException.hxx"


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Infrastructure in VOCAL related to making network transport layer 
 *  connections.<br><br>
 */
namespace Transport 
{


class Socket;
class IPAddress;


/** This class decorates a Socket with the purpose of configuring the 
 *  socket.<br><br>
 *
 *  @see    Vocal::Transport::Socket
 *  @see    Vocal::SystemException
 *  @see    Vocal::Transport::IPAddress
 */
class SocketOptions
{
    public:


    	/** Create the SocketOption, decorating the given socket.
	 */    	    	
    	SocketOptions(Socket &);


    	/** destructor
	 */    	    	
    	~SocketOptions();


    	/** Enable/disable sending of keep-alive messages on 
	 *  connection-oriented sockets.
	 *
	 *  See socket(7), and getsockopt/setsockopt(2).
	 */    	    	
    	void	    	keepAlive(bool on) 
		    	    throw ( Vocal::SystemException );


    	/** Query state of keepAlive.
	 */    	    	
	bool	    	keepAlive() const
		    	    throw ( Vocal::SystemException );


    	/** Enable BSD bug-to-bug compatibility. Linux only.
	 *
	 *  See socket(7), and getsockopt/setsockopt(2).
	 */    	    	
	void	    	bsdCompatible(bool on = true)
		    	    throw ( Vocal::SystemException );


    	/** Query state of bsdCompatibility.
	 */    	    	
	bool	    	bsdCompatible() const
		    	    throw ( Vocal::SystemException );


    	/** Indicates that the rules used in validating addresses supplied  
	 *  in a bind(2) call should allow reuse of local addresses.
	 *
	 *  See socket(7), and getsockopt/setsockopt(2).
	 */    	    	
	void	    	reuseAddress(bool on = true)
		    	    throw ( Vocal::SystemException );


    	/** Query state of reuseAddress.
	 */    	    	
	bool	    	reuseAddress() const		    
		    	    throw ( Vocal::SystemException );


    	/** When enabled, datagram sockets receive packets sent to a broadcast
    	 *  address and they are allowed to send packets to a broadcast 
	 *  address. This option has no effect on stream-oriented sockets.
	 *
	 *  See socket(7), and getsockopt/setsockopt(2).
	 */    	    	
	void	    	broadcast(bool on = true)
		    	    throw ( Vocal::SystemException );


    	/** Query state of broadcast.
	 */    	    	
	bool	    	broadcast() const
		    	    throw ( Vocal::SystemException );


    	/** When enabled, a close(2) or shutdown(2) will not return until all 
	 *  queued messages for the socket have been successfully sent or the 
	 *  linger timeout has been reached. Otherwise, the call returns 
	 *  immediately and the closing is done in the background.
	 *
	 *  See socket(7), and getsockopt/setsockopt(2).
	 */    	    	
	void	    	linger(bool on, int seconds)
		    	    throw ( Vocal::SystemException );


    	/** Query state of linger. If enabled, the seconds will return
	 *  with the linger timeout.
	 */    	    	
	bool	    	linger(int & seconds) const
		    	    throw ( Vocal::SystemException );


    	/** Join a multicast group, as given in the IPAddress.
	 *
	 *  See ip(7), and getsockopt/setsockopt(2).
	 */    	    	
	void	    	joinMulticastGroup(IPAddress & mcastAddr)
		    	    throw ( Vocal::SystemException );


    	/** Leave a multicast group, as given in the IPAddress.
	 *
	 *  See ip(7), and getsockopt/setsockopt(2).
	 */    	    	
	void	    	leaveMulticastGroup(IPAddress & mcastAddr)
		    	    throw ( Vocal::SystemException );

    private:

    	void	    	set(int level, int optname, bool)
		    	    throw ( Vocal::SystemException );


	bool	    	get(int level, int optname) const
		    	    throw ( Vocal::SystemException );
				    

    	mutable Socket	    &	socket_;
};


} // namespace Transport
} // namespace Vocal


#endif // !defined(SOCKET_OPTIONS_DOT_H)
