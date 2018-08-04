#if !defined(VOCAL_TRANSPORTADDRESS_HXX)
#define VOCAL_TRANSPORTADDRESS_HXX
/*
 * $Id: TransportAddress.hxx,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */

#include "TransportCommon.hxx"
#include "Writer.hxx"
#include "AddressFamily.hxx"
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


/** Interface class for all forms of transport addresses.<br><br>
 *
 *  @see    Vocal::Transport::IPAddress
 *  @see    Vocal::Transport::IP6Address
 *  @see    Vocal::Transport::AddressFamily
 *  @see    Vocal::Transport::Socket
 *  @see    Vocal::SystemException
 */
class TransportAddress : public Vocal::IO::Writer
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
	    	    	    	    throw ( Vocal::SystemException ) = 0;

    private:

    	/** Copying suppressed.
	 */
	TransportAddress(const TransportAddress &);


    	/** Copying suppressed.
	 */
	TransportAddress & operator=(const TransportAddress &);
};


} // namespace Transport
} // namespace Vocal


#endif // !defined(VOCAL_TRANSPORTADDRESS_HXX)
