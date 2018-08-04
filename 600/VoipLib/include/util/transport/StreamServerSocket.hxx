#if !defined(STREAM_SERVER_SOCKET_DOT_H)
#define STREAM_SERVER_SOCKET_DOT_H
/*
 * $Id: StreamServerSocket.hxx,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */


#include "Socket.hxx"
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


class StreamClientSocket;


/** Stream server socket provides an address family agnostic way to create 
 *  a socket that will listen for incoming connections.<br><br>
 *
 *  @see    Vocal::Transport::Socket
 *  @see    Vocal::Transport::TransportAddress
 *  @see    Vocal::Transport::AddressFamily
 *  @see    Vocal::SystemException
 */
class StreamServerSocket : public Vocal::Transport::Socket
{

    protected:

    	/** Address family constructor. Create a server socket whose 
	 *  local address will be a default transport address 
	 *  corresponding to the given address family. This usually 
	 *  produces an "ANY" address that will be allocated by the 
	 *  operating system.
	 */
    	StreamServerSocket( 
	    const AddressFamily     & 	addressFamily,
    	    int     	    	    	backlog = 5,
    	    const char      	    * 	name = 0
	)
    	    throw ( Vocal::SystemException );


    	/** TransportAddress constructor. Create a socket whose
	 *  local address is bound to the given local address.
	 */
    	StreamServerSocket( 
	    const TransportAddress  &	localAddr,
	    int     	    	    	backlog = 5,
	    const char      	    *   name = 0
	)
    	    throw ( Vocal::SystemException );

    public:

    	/** Virtual destructor
	 */
	virtual ~StreamServerSocket();


    private:

    	void	listen(int backlog)
    	    	    throw ( Vocal::SystemException );
};


} // namespace Transport
} // namespace Vocal


#endif // !defined(STREAM_SERVER_SOCKET_DOT_H)
