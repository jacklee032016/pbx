#if !defined(ASSIST_UDP_SOCKET_HXX)
#define ASSIST_UDP_SOCKET_HXX
/*
* $Id: UDPSocket.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "DatagramSocket.hxx"


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


/** UDP socket is a connection-oriented or connectionless
 *  abstraction for unreliable message transport, using the
 *  UDP/IP protocol.<br><br>
 *
 *  @see    Assist::Transport::DatagramSocket
 *  @see    Assist::Transport::Socket
 *  @see    Assist::Transport::IPAddress
 *  @see    Assist::Transport::TransportAddress
 *  @see    Assist::SystemException
 */
class UDPSocket : public Assist::Transport::DatagramSocket
{

    public:


    	/** Default construct with a optional name.
	 */
    	UDPSocket(const char * name = 0)
    	    throw ( Assist::SystemException );


    	/** Construct, binding to the given local address and
	 *  with a optional name.
	 */
    	UDPSocket(
	    IPAddress 	    &   localAddr, 
	    const char      *   name = 0
	)
    	    throw ( Assist::SystemException );


    	/** Construct, binding to the given local address,
	 *  connecting to the remote address and with a optional name.
	 */
    	UDPSocket(
	    IPAddress 	    &   localAddr, 
    	    IPAddress 	    &   remoteAddr,
    	    const char      *   name = 0
	)
    	    throw ( Assist::SystemException );
    					

    	/** Virtual destructor.
	 */
	virtual     	~UDPSocket();



    	/** If connected, this will return a pointer to the remote ip address.
	 *  Otherwise 0 will be returned.
    	 */
	Sptr<IPAddress>     getRemoteIPAddress() const;
};


} // namespace Transport
} // namespace Assist


#endif // !defined(ASSIST_UDP_SOCKET_HXX)
