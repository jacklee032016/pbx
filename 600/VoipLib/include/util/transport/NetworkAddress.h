#ifndef NETWORKADDRESS_HXX_
#define NETWORKADDRESS_HXX_
/*
 * $Id: NetworkAddress.h,v 1.4 2007/06/05 11:33:25 lizhijie Exp $
 */


#include <netdb.h>
#include <iostream>
#include <string>

#ifdef __FreeBSD__
#include <sys/types.h>
#endif

#include <sys/socket.h>
#include <stdio.h>

#include "global.h"
#include "vtypes.h"
#include "VException.hxx"
#include "Data.hxx"

#define IPV4_LENGTH 4

/**
   Class which implements a network address, which contains both the
   IP address and port.  Includes functionality to look up network
   addresses from hostnames or ip addresses as strings.<p>

   <b>Usage</b> 

   Here are some ways to make NetworkAddress objects:<p>

   <pre>
   NetworkAddress a("www.google.com", 80);
   
   NetworkAddress b;
   b.setHostName("www.google.com");
   b.setPort(80);
   
   // here is how you would get the IP address from a
   
   cout << a.getIpName() << endl; // prints www.google.com's ip addr.
   
   </pre>
*/
class NetworkAddress
{
    public:
	class UnresolvedException : public VException
	{
	    public:
		UnresolvedException(const string& reason, 
				    const string& file,
				    const int line);
		string getName() const { return "NetworkAddress::UnresolvedException"; }
	};
	
	/// default constructor. The host is initially set to localhost.
	explicit NetworkAddress ( int port = -1 );
	
	/** The host hostName can be in the same form as passed to
	 * setHostName()
	 */
	explicit NetworkAddress ( const Data& hostName, int port = -1 );
	
	/** takes IP or DNS with optional ":port" syntax.  Examples:
	    <P>lab1.vovida.com
	    <BR>192.168.4.5
	    <BR>lab1.vovoda.com:80
	    <BR>192.168.4.5:6000
	    
	    @param theAddress the address to set it to

        */
        void setHostName ( const Data& theAddress );

        /// set port
        void setPort ( int port );

        /// get hostname in the form "lab1.vovida.com"
        const Data& getHostName () const;

        /// get IP address as a string in the form "192.168.4.5"
        const Data& getIpName () const;

        /** 
            return the IP4 address packed as an unsigned int, in
            network byte order.
            @deprecated use getSockAddr() instead.
        */
        u_int32_t getIp4Address () const;

        /** 
            return the address in a sockaddr.
            @param socka    returned address
            @param uhints   ?
        */
        void getSockAddr(struct sockaddr_storage * socka, struct addrinfo* uhints=0) const;

        /// get the port
        int getPort () const;

        /// compare two IP addresses
        friend bool operator < (const NetworkAddress & xAddress,
                                const NetworkAddress & yAddress );

        /// compare two IP addresses
        friend bool operator == (const NetworkAddress & xAddress,
                                 const NetworkAddress & yAddress );

        /// compare two IP addresses
        friend bool operator != (const NetworkAddress & xAddress,
                                 const NetworkAddress & yAddress );

        /// convert to string - returns same as print output; i.e. host:port
        operator string() const;

        /// printing operator -- calls print() fn.
        friend ostream & operator<< ( ostream & xStream,
                                      const NetworkAddress& rxObj );

        /// copy constructor
        NetworkAddress( const NetworkAddress& x);

        /// assignment operator =
        NetworkAddress& operator=( const NetworkAddress& x );

        /// printing operator.  This should be replaced w/ Vocal::IO:Writer
        ostream & print ( ostream & xStr ) const;

        /** hash function, uses last 2 bytes of ip address + 2 bytes
            of port the bits 15-0 of ipaddress (Bigendian) becomes the
            Most Significant Byte(MSB) of hashkey, bits 15-0 of port
            becomes LSB of hashkey.
        */
        u_int32_t hashIpPort( ) const;

        /// hash function, uses last 2 bytes of ip address + 2 bytes of port
        static u_int32_t 
        hashIpPort( const u_int32_t ipAddress, const int port );

        /** Mutexes the gethostbyname(), return 0 when successful
         * the last argumrnt is only for Windows protability
         */
        static int getHostByName(const char* hostName,
                                 struct addrinfo* res,
                                 struct hostent* hEnt=0);

        ///calls unix gethostname()
        static Data getLocalHostName();

        static void freeAddrInfo(struct addrinfo* res);

      
	/// value if host lookup failed
        static const int getHostLookupFailed;
        /// value if host lookup succeeded
        static const int getHostLookupOK;

        ///Resolves an IP address to a name
        Data getHostByAddress(Data& ipAddr) const;

        ///Check if the passed arg is an IpV6 address
        static bool is_valid_ip6_addr (const Data& ip_addr);

    private:
        ///
        static bool is_v6_string(const Data& addr);

        /// Check if parameter is a valid IP address
        bool is_valid_ip_addr (const Data& ip_addr) const;
        bool is_valid_ip4_addr (const Data& ip_addr) const;

        void init(string& hostname, int port) const;
        void initIpAddress() const;
        bool isDottedDecimal(const string& addr);
    
        mutable int aPort;
        mutable Data hostName;
        mutable Data ipAddress;
        mutable sockaddr_storage sa_cache;
        mutable bool ipAddressSet;
        mutable bool sockAddrSet;
        static string localHostName;
};


inline ostream &operator<< ( ostream & xStream, const NetworkAddress& rxObj )
{
    return rxObj.print ( xStream );
}


class NetworkAddressHash
{
    public:
        unsigned long int operator() ( const NetworkAddress& src ) const
            {
                return src.hashIpPort();
            }
};

// NETWORKADDRESS_HXX_
#endif

