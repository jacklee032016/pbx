#if !defined(ASSIST_TRANSPORTADDRESS_HXX)
#define ASSIST_TRANSPORTADDRESS_HXX
/*
* $Id: TransportAddress.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "TransportCommon.hxx"
#include "Writer.hxx"
#include "AddressFamily.hxx"
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


/** Interface class for all forms of transport addresses.<br><br>
 *
 *  @see    Assist::Transport::IPAddress
 *  @see    Assist::Transport::IP6Address
 *  @see    Assist::Transport::AddressFamily
 *  @see    Assist::Transport::Socket
 *  @see    Assist::SystemException
 */
class TransportAddress : public Assist::IO::Writer
{

    protected:


    	/** Default construction.
	 */
    	TransportAddress();


    public:				


    	/** Virtual destructor.
	 */
    	virtual ~TransportAddress();


    	/** Factory method to create a transport address given
	 *  the address family or clone the current address.
	 */
    	static	TransportAddress *  create(int addressFamily);


    	/** Factory method to clone this transport address.
	 */
	virtual TransportAddress *  clone() const = 0;


        /** Clear the address
         */
        virtual void        clear() = 0;


    protected:


    	/** Set the socket address.
	 */
    	virtual void	    	    setAddress(sockaddr *) = 0;
	

	/** Set the address family.
	 */
	void	    	    	    setAddressFamily(int);
	

    public:


    	/** Get the address in the form of a sockaddr.
	 */	
    	virtual sockaddr *  	    getAddress() const = 0;


    	/** Get the address length.
	 */	
   	virtual socklen_t   	    getAddressLength() const = 0;
	

	/** Get the address family.
	 */
	AddressFamily	    	    getAddressFamily() const;


    	/** Update the address from the socket.
	 */
	virtual void	    	updateAddress(const Socket &)
	    	    	    	    throw ( Assist::SystemException ) = 0;

    private:

    	/** Copying suppressed.
	 */
	TransportAddress(const TransportAddress &);


    	/** Copying suppressed.
	 */
	TransportAddress & operator=(const TransportAddress &);
};


} // namespace Transport
} // namespace Assist


#endif // !defined(ASSIST_TRANSPORTADDRESS_HXX)
