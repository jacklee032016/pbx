#if !defined(VOCAL_UDP_SOCKET_HXX)
#define VOCAL_UDP_SOCKET_HXX
/*
 * $Id: UDPSocket.hxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */


#include "DatagramSocket.hxx"


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


/** UDP socket is a connection-oriented or connectionless
 *  abstraction for unreliable message transport, using the
 *  UDP/IP protocol.<br><br>
 *
 *  @see    Vocal::Transport::DatagramSocket
 *  @see    Vocal::Transport::Socket
 *  @see    Vocal::Transport::IPAddress
 *  @see    Vocal::Transport::TransportAddress
 *  @see    Vocal::SystemException
 */
class UDPSocket : public Vocal::Transport::DatagramSocket
{

    public:


    	/** Default construct with a optional name.
	 */
    	UDPSocket(const char * name = 0)
    	    throw ( Vocal::SystemException );


    	/** Construct, binding to the given local address and
	 *  with a optional name.
	 */
    	UDPSocket(
	    IPAddress 	    &   localAddr, 
	    const char      *   name = 0
	)
    	    throw ( Vocal::SystemException );


    	/** Construct, binding to the given local address,
	 *  connecting to the remote address and with a optional name.
	 */
    	UDPSocket(
	    IPAddress 	    &   localAddr, 
    	    IPAddress 	    &   remoteAddr,
    	    const char      *   name = 0
	)
    	    throw ( Vocal::SystemException );
    					

    	/** Virtual destructor.
	 */
	virtual     	~UDPSocket();



    	/** If connected, this will return a pointer to the remote ip address.
	 *  Otherwise 0 will be returned.
    	 */
	Sptr<IPAddress>     getRemoteIPAddress() const;
};


} // namespace Transport
} // namespace Vocal


#endif // !defined(VOCAL_UDP_SOCKET_HXX)
