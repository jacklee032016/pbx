#if !defined(STREAM_SERVER_SOCKET_DOT_H)
#define STREAM_SERVER_SOCKET_DOT_H
/*
* $Id: StreamServerSocket.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "Socket.hxx"
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


class StreamClientSocket;


/** Stream server socket provides an address family agnostic way to create 
 *  a socket that will listen for incoming connections.<br><br>
 *
 *  @see    Assist::Transport::Socket
 *  @see    Assist::Transport::TransportAddress
 *  @see    Assist::Transport::AddressFamily
 *  @see    Assist::SystemException
 */
class StreamServerSocket : public Assist::Transport::Socket
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
    	    throw ( Assist::SystemException );


    	/** TransportAddress constructor. Create a socket whose
	 *  local address is bound to the given local address.
	 */
    	StreamServerSocket( 
	    const TransportAddress  &	localAddr,
	    int     	    	    	backlog = 5,
	    const char      	    *   name = 0
	)
    	    throw ( Assist::SystemException );

    public:

    	/** Virtual destructor
	 */
	virtual ~StreamServerSocket();


    private:

    	void	listen(int backlog)
    	    	    throw ( Assist::SystemException );
};


} // namespace Transport
} // namespace Assist


#endif // !defined(STREAM_SERVER_SOCKET_DOT_H)
