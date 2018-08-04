#ifndef UDP_HXX
#define UDP_HXX
/*
* $Id: UdpStack.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"
#include <fstream>
#include <string>
#include <assert.h>
#include "NetworkAddress.h"
#ifdef __vxworks
#include <selectLib.h>
#else
#include <sys/select.h>
#endif
#include "VMutex.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

///
typedef enum
{
    inactive,
    sendonly,
    recvonly,
    sendrecv
} UdpMode;


// class UdpStackPrivateData;
// this class hold stuff that is likely to be a problem to port
class UdpStackPrivateData
{
    public:
	UdpStackPrivateData() {
		localAddr = new sockaddr_storage();
		remoteAddr = new sockaddr_storage();
	}
        /// File descriptor of socket
        int socketFd;

        /// Address of local computer that will receive or send the packets
        struct sockaddr_storage *localAddr;

        /** Address of remote computer that local computer receive from
         * or send to the packets
         * This address is set when doClient is called.
         * And later when setDestination() is called, this address will be the one
         * connect to. But disconnectPorts() won't remove this address.
         */
        struct sockaddr_storage *remoteAddr;

};



/** 
    A UDP transmitter and reciever.<p>

    This class functions to send and receive UDP packets over the
    network.  It is a simple abstraction of the standard facilities,
    in an slightly OO way.<p>

    By default, UdpStack prepares to both send and receive UDP packets.<p>

    <b>Receive Example:</b><p>

    <pre>
    UdpStack udpStack( 0, 5060 ); // listen on port 5060
    
    NetworkAddress sender;
    char buf[1024];
    
    // read a packet from the network, or timeout after 1 second
    int len = udpStack.receiveTimeout(buf, 1024, &sender, 1, 0);
    </pre>

    <p>
    <b>Send Example:</b><p>
    
    <pre>
    UdpStack udpStack( 0, 5060 ); // listen on port 5060
    
    NetworkAddress target;
    
    // ... stuff to set target here
    
    char buf[1024];
    strcpy(buf, "test");

    // read a packet from the network, or timeout after 1 second
    int len = udpStack.transmitTo(buf, strlen(buf) + 1, &target);
    </pre>

    <p>
    <b>Issues</b><p>

    Currently there is  no support for saying which interface a packet
    arrived on. I'm not sure why that would be useful but I think
    someone requested it once.
*/

class UdpStack
{
    public:
        /** no ports can be specified and the stack will pick one, OR a minPort
            can be specified and it will be used, OR a min and max can be
            specified and the stack will use the first free port in that
            range. The destiantion hostname and port may be set in the
            destinationHost parameter **/
        UdpStack ( const NetworkAddress* destinationHost = NULL,
                   int localMinPort = -1,
                   int localMaxPort = -1,
                   UdpMode mode = sendrecv,
                   bool log_flag = false,
                   bool isMulticast = false);

        /** connectPorts() has to be called before using send()
         * It associates the local port to remote port
         * so that receive and send only from and to that remote port
	 */
        void connectPorts();

        /// disconnectPorts() dissolve the port association
        void disconnectPorts();

        /// set the local ports
        void setLocal (const int minPort = -1, int maxPort = -1 );

        /// set the default destination
        void setDestination ( const NetworkAddress* host );

        /// set the default destination
        void setDestination ( const char* host, int port );

        /// Get the udp stack mode
        UdpMode getMode ()
        {
            return mode;
        };

        /// Get the udp stack logFlag
        bool getLogFlag ()
        {
            return logFlag;
        };

        /// Get the name for local
        string getLclName () const
        {
            return lclName;
        };

        /// Get the name for remote
        string getRmtName () const
        {
            return rmtName;
        };

        /// Get the port being used for recieving
        int getRxPort ();

        /// Get the port being used for transmitting
        int getTxPort ();

        /// Get the destination port
        int getDestinationPort ()
        {
            return getRxPort();
        };

        ///
        NetworkAddress* getDestinationHost () const;


        /// Set the mode
        void setMode ( const UdpMode theMode )
        {
            mode = theMode;
        };

        /// Set the logFlag
        void setLogFlag ( const bool theLogFlag )
        {
            logFlag = theLogFlag;
        };

        /// Set the name to something logical - only used for errors
        void setLclName ( const string& theName )
        {
            lclName = theName;
        };

        void setRmtName ( const string& theName )
        {
            rmtName = theName;
        };


        /// Get the file descriptor for the socket - use with care or not at all
        int getSocketFD ();

        /// Add this stacks file descriptors to the the fdSet
        void addToFdSet ( fd_set* set );

        /// Find the max of any file descripts in this stack and the passed value
        int getMaxFD ( int prevMax = 0 );

        /// Check and see if this stacks file descriptor is set in fd_set
        bool checkIfSet ( fd_set* set );


        /** 
	    Receive a datagram.
            Once connectPorts() is called, can't receive from other ports.

	    @param buffer   buffer to write message into
	    @param bufSize     maximum size of the buffer
	    @return size of message received.
	*/
        int receive ( const char* buffer,
                      const int bufSize );  // returns bytes read

        /** 
	    Receive a datagram.
            Use receive if you do not need to be able to tell who this packet
            came from.

	    @param buffer   buffer to write message into
	    @param bufSize  maximum size of the buffer
	    @param sender   address of the sender.
	    @return size of message received.

	*/
        int receiveFrom ( const char* buffer,
                          const int bufSize,
                          NetworkAddress* sender );  // returns bytes read

        /** 
	    Receive a datagram.
            Use receive if you do not need to be able to tell who this packet
            came from.

	    @param buffer   buffer to write message into.
	    @param bufSize     maximum size of the buffer.
	    @param sender   the NetworkAdddress# of the sender.
	    @param sec      number of seconds to wait.
	    @param usec     number of microseconds to wait.

	    @return size of message received, if successful, -1 for
	    error, or 0 if the timeout was reached without receiving a
	    message.
	*/
        int receiveTimeout ( const char* buffer,
                             const int bufSize,
                             NetworkAddress* sender,
                             int sec = 0,
                             int usec = 0);

        /** 
	    Transmit a datagram.
            ConnectPorts() should be called before it is called.

	    @param buffer   buffer to read message from
	    @param length  number of bytes to send.
	*/
        void transmit ( const char* buffer,
                        const int length );

        /** 
	    Transmit a datagram.
	    Use transmit if you always sent to the same person.

	    @param buffer   buffer to read message from.
	    @param length   number of bytes to send.
	    @param dest     destination to send packet to.

	    @return 0 if transmitted OK, or > 0 if not OK.
	*/
        /// Transmit a datagram
        int transmitTo ( const char* buffer,
                         const int length,
                         const NetworkAddress* dest );
        ///
        void joinMulticastGroup ( NetworkAddress group,
                                  NetworkAddress* iface = NULL,
                                  int ifaceInexe = 0 );
        ///
        void leaveMulticastGroup( NetworkAddress group,
                                  NetworkAddress* iface = NULL,
                                  int ifaceInexe = 0 );
        ///
        virtual ~UdpStack ();

        /// Get the number of bytes the stack has processed
        int getBytesTransmitted () const;

        /// Get the number of datagrams the stack has processed
        int getPacketsTransmitted () const;

        /// Get the number of bytes the stack has processed
        int getBytesReceived () const;

        /// Get the number of datagrams the stack has processed
        int getPacketsReceived () const;

        /** This can be used to eulate network packet loss by throwing
            out a certain percentage of the tranmitted packets */
        void emulatePacketLoss ( float probabilityOfLoss )
        {
            packetLossProbability = probabilityOfLoss;
        };

	/**Set the mode to be blocking or non-blocking
	  *flg=true (blocking)
	  *flg=false (non-blocking), default beaviour is blocking
          */
	int setModeBlocking(bool flg);

    private:
        UdpStack& operator= ( const UdpStack& x )
        {
            assert ( 0 );
            return *this;
        };

        UdpStack( const UdpStack& )
        {
            assert ( 0 );
        };

        void doServer ( int minPort,
                        int maxPort );

        void doClient ( const NetworkAddress* desName);

        int recvfrom_flags(int fd, void *ptr, size_t nbytes, int *flagsp,
                     struct sockaddr *sa, socklen_t *salenptr, struct in6_pktinfo *pktp);
        /// name of receiver
        string lclName;

        /// name of transmitter
        string rmtName;

        /// probability of a given packet being thrown out by the stack
        float packetLossProbability;

        /// number of bytes the stack has received
        int numBytesReceived;

        /// number of datagrams the stack has received
        int numPacketsReceived;

        /// number of bytes the stack has transimitted
        int numBytesTransmitted;

        /// number of datagrams the stack has transimitted
        int numPacketsTransmitted;

        /// Mode
        UdpMode mode;

        /// flag for msg log
        bool logFlag;

        /// private data that will be a pain to port
        UdpStackPrivateData* data;

        ofstream* in_log;
        ofstream* out_log;

        int rcvCount ;
        int sndCount ;
        VMutex _lock;
	bool   blockingFlg;
};


/// Class to hold all types of exceptions that occur in the UDP stack
class UdpStackException
{
    public:
        ///
        UdpStackException ( const string& description )
        : desc(description)
        {}
        ;

        string getDescription() const
        {
            return desc;
        }
    private:
        string desc;

        friend std::ostream& operator<< ( std::ostream& strm , const UdpStackException& e );
};

inline std::ostream&
operator<< ( std::ostream& strm , const UdpStackException& e )
{
    strm << e.desc;
    return strm;
}

//e exception class for when a transmited packet is renused
class UdpStackExceptionConectionRefused: public UdpStackException
{
    public:
        ///
        UdpStackExceptionConectionRefused( const string& description )
        : UdpStackException(description)
        {}
        ;
};


/// exception class for when all ports all already in use
class UdpStackExceptionPortsInUse: public UdpStackException
{
    public:
        ///
        UdpStackExceptionPortsInUse ( const string& description )
        : UdpStackException(description)
        {}
        ;
};


/// exception class for when a bogus hostname is given
class UdpStackExceptionBadHostname: public UdpStackException
{
    public:
        ///
        UdpStackExceptionBadHostname( const string& description )
        : UdpStackException(description)
        {}
        ;
};


/// exception class for when a bogus hostname is given
class UdpStackExceptionBadPort: public UdpStackException
{
    public:
        ///
        UdpStackExceptionBadPort( const string& description )
        : UdpStackException(description)
        {}
        ;
};



#endif
