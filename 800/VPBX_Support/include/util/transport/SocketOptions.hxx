#if !defined(SOCKET_OPTIONS_DOT_H)
#define SOCKET_OPTIONS_DOT_H
/*
* $Id: SocketOptions.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "SystemException.hxx"


/** Infrastructure common to ASSIST.
 */
namespace Assist 
{


/** Infrastructure in ASSIST related to making network transport layer 
 *  connections.<br><br>
 */
namespace Transport 
{


class Socket;
class IPAddress;


/** This class decorates a Socket with the purpose of configuring the 
 *  socket.<br><br>
 *
 *  @see    Assist::Transport::Socket
 *  @see    Assist::SystemException
 *  @see    Assist::Transport::IPAddress
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
		    	    throw ( Assist::SystemException );


    	/** Query state of keepAlive.
	 */    	    	
	bool	    	keepAlive() const
		    	    throw ( Assist::SystemException );


    	/** Enable BSD bug-to-bug compatibility. Linux only.
	 *
	 *  See socket(7), and getsockopt/setsockopt(2).
	 */    	    	
	void	    	bsdCompatible(bool on = true)
		    	    throw ( Assist::SystemException );


    	/** Query state of bsdCompatibility.
	 */    	    	
	bool	    	bsdCompatible() const
		    	    throw ( Assist::SystemException );


    	/** Indicates that the rules used in validating addresses supplied  
	 *  in a bind(2) call should allow reuse of local addresses.
	 *
	 *  See socket(7), and getsockopt/setsockopt(2).
	 */    	    	
	void	    	reuseAddress(bool on = true)
		    	    throw ( Assist::SystemException );


    	/** Query state of reuseAddress.
	 */    	    	
	bool	    	reuseAddress() const		    
		    	    throw ( Assist::SystemException );


    	/** When enabled, datagram sockets receive packets sent to a broadcast
    	 *  address and they are allowed to send packets to a broadcast 
	 *  address. This option has no effect on stream-oriented sockets.
	 *
	 *  See socket(7), and getsockopt/setsockopt(2).
	 */    	    	
	void	    	broadcast(bool on = true)
		    	    throw ( Assist::SystemException );


    	/** Query state of broadcast.
	 */    	    	
	bool	    	broadcast() const
		    	    throw ( Assist::SystemException );


    	/** When enabled, a close(2) or shutdown(2) will not return until all 
	 *  queued messages for the socket have been successfully sent or the 
	 *  linger timeout has been reached. Otherwise, the call returns 
	 *  immediately and the closing is done in the background.
	 *
	 *  See socket(7), and getsockopt/setsockopt(2).
	 */    	    	
	void	    	linger(bool on, int seconds)
		    	    throw ( Assist::SystemException );


    	/** Query state of linger. If enabled, the seconds will return
	 *  with the linger timeout.
	 */    	    	
	bool	    	linger(int & seconds) const
		    	    throw ( Assist::SystemException );


    	/** Join a multicast group, as given in the IPAddress.
	 *
	 *  See ip(7), and getsockopt/setsockopt(2).
	 */    	    	
	void	    	joinMulticastGroup(IPAddress & mcastAddr)
		    	    throw ( Assist::SystemException );


    	/** Leave a multicast group, as given in the IPAddress.
	 *
	 *  See ip(7), and getsockopt/setsockopt(2).
	 */    	    	
	void	    	leaveMulticastGroup(IPAddress & mcastAddr)
		    	    throw ( Assist::SystemException );

    private:

    	void	    	set(int level, int optname, bool)
		    	    throw ( Assist::SystemException );


	bool	    	get(int level, int optname) const
		    	    throw ( Assist::SystemException );
				    

    	mutable Socket	    &	socket_;
};


} // namespace Transport
} // namespace Assist


#endif // !defined(SOCKET_OPTIONS_DOT_H)
