#if !defined(IPADDRESS_DOT_H)
#define IPADDRESS_DOT_H
/*
 * $Id: IPAddress.hxx,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */


#include "TransportAddress.hxx"
#include <netinet/in.h>
#include <string>


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


class SystemException;


/** Infrastructure in VOCAL related to making network transport layer 
 *  connections.<br><br>
 */
namespace Transport 
{


/** IP version 4 style address, extends TransportAddress.<br><br>
 *
 *  @see    Vocal::Transport::TransportAddress
 *  @see    Vocal::Transport::IP6Address
 *  @see    Vocal::Transport::AddressFamily
 *  @see    Vocal::Transport::Socket
 *  @see    Vocal::SystemException
 */
class IPAddress : public Vocal::Transport::TransportAddress
{
    public:


    	/** Construct given the optional port number.
	 */
	IPAddress(u_int16_t 	    	port = 0);
	

    	/** Construct given the ip in dotted decimal, C string format,
	 *  and an optional port.
	 */
	IPAddress(const char 	    *	ip,
		  u_int16_t 	    	port = 0);


    	/** Construct given the ip in an array of octets, each octet
	 *  representing a portion of the address and an optional port.
	 *  <br><br>
	 *
	 *  Note that the size of the array must be at least 4 bytes long,
	 *  otherwise the operation is unknown, aka don't call me if
	 *  it crashes in this code thanks to a bogus array that you passed
	 *  in.
	 */
	IPAddress(const u_int8_t    *   ip,
		  u_int16_t 	    	port = 0);


    	/** Construct given an ip as a host ordered long, and a port.
	 */
	IPAddress(u_int32_t    	    	ip,
		  u_int16_t 	    	port);


    	/** Construct given a sockaddr_in.
	 */
	IPAddress(const sockaddr_in &   inAddr);


    	/** Copy constructor.
	 */
	IPAddress(const IPAddress &);
			    

    	/** Virtual destructor.
	 */
	virtual ~IPAddress();
	

	/** Assignment operator
	 */
	IPAddress & 	    	operator=(const IPAddress &);


    	/** Factory method. clone the current address.
	 */
	virtual TransportAddress  *	clone() const;
		

    	/** Set the socket address.
	 */
    	virtual void	    	setAddress(sockaddr *);


	/** Set the address by sockaddr_in
	 */
	void	    	    	setIPAddress(const sockaddr_in &);
	
	
	/** Set IP portion of address by dotted decimal, C string.
	 */
	void	    	    	setIP(const char *);


	/** Set IP portion of address by dotted decimal, C string.
	 */
	void	    	    	setIP(const string &);


	/** Set IP portion of address by octet sting. Must be at least
	 *  4 octets long.
	 */
	void	    	    	setIP(const u_int8_t *longForm);


	/** Set IP portion of address by host ordered unsigned long.
	 */
	void	    	    	setIP(u_int32_t);
	

	/** Set port portion of address.
	 */
	void	    	    	setPort(u_int16_t);
    

	/** Get IP portion of the address by dotted decimal notation.
	 */
	string	    	    	getIPAsString() const;


	/** Get IP portion of the address by octet sting. 
	 *  Assuming dst points to 4-byte memory area.
	 */
	void     	        getIPAsOctetString(u_int8_t* dst) const;


	/** Get IP portion of the address as host ordered unsigned long.
	 */
	u_int32_t               getIPAsULong() const;


	/** Get IP and port  by dotted decimal notation followed by a colon
	 *  followed by a port, e.g. "192.168.5.23:1719".
	 */
    	string	    	    	getIPAndPortAsString() const;
		

    	/** Get the address in the form of a sockaddr.
	 */	
    	virtual sockaddr *  	getAddress() const;


    	/** Get the entire address as a sockaddr_in.
	 */
    	const sockaddr_in &	getIPAddress() const;


    	/** Get the native sockaddr's length.
	 */
   	virtual socklen_t   	getAddressLength() const;
	

	/** Get the port portion of the address.
	 */
	u_int16_t   	    	getPort() const;


    	/** Update the address from the socket.
	 */
	virtual void	    	updateAddress(const Socket &)
	    	    	    	    	throw ( Vocal::SystemException );

    	/** Equality relational operator
	 */
	bool	    	    	operator==(const IPAddress &) const;


    	/** Inequality relational operator
	 */
	bool	    	    	operator!=(const IPAddress &) const;


    	/** Less than relational operator
	 */
	bool	    	    	operator<(const IPAddress &) const;


    	/** Less than  or equal to relational operator
	 */
	bool	    	    	operator<=(const IPAddress &) const;


    	/** Greater than relational operator
	 */
	bool	    	    	operator>(const IPAddress &) const;


    	/** Greater than or equal to relational operator
	 */
	bool	    	    	operator>=(const IPAddress &) const;


    	/** Returns true if the addr portion is INADDR_ANY and the
    	 *  port is zero.
	 */
    	bool	    	    	isAny() const;
	

        /** Clears the ip address
         */
        void                    clear();
        
        
    	/** Print the address to a stream.
	 */	
	ostream &	    	writeTo(ostream &) const;
	

    private:

    	char *	    	    	inetNtoa(const struct in_addr & in, char *) const;
	    
    	mutable sockaddr_in 	inAddr_;
        
        char                    separationChar_;
};


} // namespace Transport
} // namespace Vovida


#endif // !defined(IPADDRESS_DOT_H)
