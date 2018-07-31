#if !defined(SOCKET_DOT_H)
#define SOCKET_DOT_H
/*
* $Id: Socket.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "TransportCommon.hxx"
#include "FileDescriptor.hxx"
#include "AddressFamily.hxx"
#include "SocketType.hxx"
#include "SystemException.hxx"
#include <string>


/** Infrastructure common to ASSIST.
 */
namespace Assist 
{


/** Infrastructure in ASSIST related to making network transport layer 
 *  connections.<br><br>
 */
namespace Transport 
{


class AddressFamily;
class TransportAddress;
class SocketType;


/** A Socket extends a FileDescriptor, specializing it with
 *  transport related properties like an address type, a socket type
 *  and a local transport address.<br><br>
 *
 *  @see    Assist::IO::FileDescriptor
 *  @see    Assist::Transport::AddressFamily
 *  @see    Assist::Transport::TransportAddress
 *  @see    Assist::Transport::SocketType
 *  @see    Assist::SystemException
 *  @see    Assist::IO::file_descriptor_t
 */
class Socket : public Assist::IO::FileDescriptor
{
    protected:


    	/** Construct given the AddressFamily, SocketType, the optional
	 *  name, and an optional already existing native file descriptor.
	 *  This will create the socket and bind the socket to the next
	 *  available port.
	 *
	 *  @exception	Assist::SystemException
	 */
    	Socket(
	    const AddressFamily     & 	addressFamily,
	    const SocketType 	    &	sockType,
	    const char      	    * 	name = 0,
	    IO::file_descriptor_t   	fd = INVALID
	)
    	throw ( Assist::SystemException );


    	/** Construct given the local transport address, the SocketType
	 *  and the optional socket name. The socket will be created and
	 *  it will be bound to the local transport address.
	 *
	 *  @exception	Assist::SystemException
	 */
    	Socket(
	    const TransportAddress  & 	localAddr,
	    const SocketType 	    & 	sockType,
	    const char      	    * 	name = 0
	)
	throw ( Assist::SystemException );

    public:
    					
    	/** Virtual destructor.
	 */
	virtual ~Socket();


    	/** Returns the local address to which the socket is bound.
	 */
    	const 	TransportAddress &  	getLocalAddress() const;


    	/** Returns the address family of the socket.
	 */
	const	AddressFamily &     	getAddressFamily() const;


    	/** Returns the socket type.
	 */
    	const	SocketType &	    	getSocketType() const;    


    	/** Returns the number of bytes sent on this socket.
	 */
    	unsigned long	    	    	bytesSent() const;


    	/** Returns the number of bytes received on this socket.
	 */
	unsigned long	    	    	bytesReceived() const;


    	/** Write this Socket to an ostream.
	 */
	virtual ostream &           	writeTo(ostream &) const;
    

    protected:


    	/** The local address this socket is bound to.
	 */
    	TransportAddress    	    *	localAddr_;


    	/** The addressFamily of this socket.
	 */
	AddressFamily	    	    	addressFamily_;


    	/** The socket type.
	 */
	SocketType  	    	    	socketType_;


    	/** The number of bytes sent.
	 */
	unsigned long	    	    	totalBytesSent_;


    	/** The number of bytes received.
	 */
	unsigned long	    	    	totalBytesReceived_;


    	/** The name of the socket.
	 */
    	string	    	    	    	name_;

    private:

    	/** Copying is suppressed.
	 */
	Socket(const Socket &);
	

    	/** Copying is suppressed.
	 */
    	Socket &    	    	    	operator=(const Socket &);					
};


} // namespace Transport
} // namespace Assist


#endif // !defined(SOCKET_DOT_H)
