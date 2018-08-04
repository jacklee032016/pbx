#if !defined(IP6ADDRESS_DOT_H)
#define IP6ADDRESS_DOT_H
/*
* $Id: IP6Address.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "TransportAddress.hxx"
#include <netinet/in.h>


/** Infrastructure common to ASSIST.
 */
namespace Assist 
{


/** Infrastructure in ASSIST related to making network transport layer 
 *  connections.<br><br>
 */
namespace Transport 
{


#if defined(__linux__)

/** IP version 6 style address, extends TransportAddress.<br><br>
 *
 *  @see    Assist::Transport::TransportAddress
 *  @see    Assist::Transport::IPAddress
 *  @see    Assist::Transport::AddressFamily
 *  @see    Assist::Transport::Socket
 *  @see    Assist::SystemException
 */
class IP6Address : public Assist::Transport::TransportAddress
{


    public:


    	/** Construct given the optional port number.
	 */
    	IP6Address(u_int16_t 	    	    port = 0);
	

    	/** Construct given a sockaddr_in6.
	 */
	IP6Address(const sockaddr_in6 &     in6Addr);


    	/** Copy constructor.
	 */
	IP6Address(const IP6Address 	    &);
		

    	/** Virtual destructor.
	 */	    
	virtual ~IP6Address();


    	/** Assignment operator
	 */	
	IP6Address & 	    	operator=(const IP6Address &);


    	/** Factory method. Clone the current address.
	 */
	virtual TransportAddress *  	clone() const;
		

    	/** Set the socket address.
	 */
    	virtual void	    	setAddress(sockaddr *);


	/** Set entire address by sockaddr_in6
	 */
	void	    	    	setIP6Address(const sockaddr_in6 &);
	

	/** Set port portion of the address.
	 */
	void	    	    	setPort(u_int16_t);
    

    	/** Get the address in the form of a sockaddr.
	 */	
    	virtual sockaddr *  	getAddress() const;


    	/** Get the native address.
	 */
    	const sockaddr_in6 &  	getIP6Address() const;


    	/** Get the length of the native address.
	 */
   	virtual socklen_t   	getAddressLength() const;
	

	/** Get the port portion of the address.
	 */
	u_int16_t   	    	getPort() const;


    	/** Update the address from the socket.
	 */
	virtual void	    	updateAddress(const Socket &)
	    	    	    	    throw ( Assist::SystemException );

    	/** Equality relational operator
	 */
	bool	    	    	operator==(const IP6Address &) const;


    	/** Inequality relational operator
	 */
	bool	    	    	operator!=(const IP6Address &) const;


    	/** Less than relational operator
	 */
	bool	    	    	operator<(const IP6Address &) const;


    	/** Less than or equal relational operator
	 */
	bool	    	    	operator<=(const IP6Address &) const;


    	/** Greater than relational operator
	 */
	bool	    	    	operator>(const IP6Address &) const;


    	/** Greater than or equal to relational operator
	 */
	bool	    	    	operator>=(const IP6Address &) const;


        /** Clear the address
         */
        void                    clear();
        
	
	/** Write the address to a stream.
	 */
	ostream &	    	writeTo(ostream &) const;
	

    private:
    

    	mutable sockaddr_in6	in6Addr_;
};
#endif // defined(__linux__)


} // namespace Transport
} // namespace Assist


#endif // !defined(IP6ADDRESS_DOT_H)
