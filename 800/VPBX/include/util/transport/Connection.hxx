#ifndef CONNECTION_HXX_
#define CONNECTION_HXX_
/*
* $Id: Connection.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "vin.h"
#include "global.h"
#include <string.h>

#include <string>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>

#if defined(__svr4__) || defined(__SUNPRO_CC)

#include <netinet/in.h>
#include <arpa/nameser.h>
#include <resolv.h>
#endif

#ifdef __APPLE__
typedef int socklen_t;
#include <netinet/in.h>
#endif

#include "VNetworkException.hxx"


class TcpServerSocket;
class TcpClientSocket;

typedef struct sockaddr SA;
#define MAXLINE   256

/**
   A stream based Connection object.  Network connections are
   represented by these objects.<p>

   <b>Usage</b>

   assume for the moment that you have a valid Connection named conn
   (see TcpClientSocket or TcpServerSocket for details on how to
   create a valid Connection).<p>

   <pre>
   
   // to write
   // note -- this does NOT write the trailing NULL
   conn.writeData("test\n", 5);
   
   // to read
   
   char buf[256];
   int n;

   // this reads a line as opposed to a certain number of bytes.
   int err = conn.readLine(buf, 255, n);
   
   </pre>

*/
class Connection
{
    public:

        /**
           Construct a connection.
           @param blocking  create a blocking object

        */
        Connection(bool blocking = true);

        /**
           Construct a connection.
           @param conId  file descriptor to wrap with connection object.
           @param blocking  create a blocking object.
        */
        Connection(int conId, bool blocking = true);

        /** return the file descriptor */
        inline int getConnId() const
        {
            return _connId;
        }

        /** return the address connecting to */
        inline struct sockaddr& getConnAddr()
        {
            return *_connAddr;
        }
 
        /** return length of the connecting address. */
        inline socklen_t getConnAddrLen() const
        {
            return _connAddrLen;
        }

        Connection(const Connection& other);

        virtual ~Connection();

        Connection& operator=(const Connection& other);

        /**
           this is true if the two Connection objects have the same
           fd, false otherwise.
        */

        bool operator==(const Connection& other)
        {
            return (_connId == other._connId);
        }

        /**
           this is false if the two Connection objects have the same
           fd, true otherwise.
        */

        bool operator!=(const Connection& other)
        {
            return (_connId != other._connId);
        }

        /**
           Reads line until '\n' is encountered or data ends.

           @param data buffer to read data into
           @param maxlen  maximum number of bytes to read into buffer
           @param bytesRead  set to number of bytes read.

           @return 0 if no more data, or number of bytes read, or -1
           if connection is already closed

           @throw VNetworkException
         */
        int readLine(void* data, size_t maxlen, int &bytesRead) throw (VNetworkException&);

        /**
           Reads bytes from the connection.

           @param data buffer to read data into
           @param nchar  maximum number of bytes to read into buffer
           @param bytesRead  set to number of bytes read.

           @return 0 if no more data, or number of bytes read, or -1
           if connection is closed

           @throw VNetworkException
         */
        int readn(void* data, size_t nchar, int &bytesRead) throw (VNetworkException&);



        /**
           Reads bytes from the connection.

           @param data buffer to read data into
           @param nchar  maximum number of bytes to read into buffer

           @return 0 if no more data, or number of bytes read, or -1
           if connection is closed

           @throw VNetworkException
         */
        int readn(void *data, size_t nchar) throw (VNetworkException&);


        /** 
            Writes bytes to the connection.

            @param data buffer to write data from
            @param n  number of bytes to write.

            @throw VNetworkException
        */
        void writeData(void* data, size_t n) throw (VNetworkException&);


        /** 
            Writes bytes to the connection.

            @param string string to write to the network.

            @throw VNetworkException
        */
        void writeData(string& data) throw (VNetworkException&);


        /** 
            Gets the connection description.

            @return connection description (far end) in the format
            IP_ADDRESSS:Port.
        */
        string getDescription() const;

        /** 
            Gets the IP of the destination.
            
            @return IP of the destination.
        */
        string getIp() const;

        ///Gets the port of the destination 
        int getPort() const;


        /// Still connected?  true if so.
        bool isLive() const
        {
            return (_live);
        }

        /// close connection.
        int close();


        /** 
            Check if data is ready to be read.

            @param seconds number of seconds to wait for data to be read.
            @param mSeconds number of microseconds to wait for data to be read.

            @return true if data is ready to be read, or false otherwise.
        */
        bool isReadReady(int seconds = 0, int mSeconds = 20000) const;

        /// initialize the SIGPIPE signal handler (for broken pipes)
        void initialize();

        /// handler for SIGPIPE signal
        static void signalHandler(int signo);

        /// @deprecated
        void deepCopy(const Connection& src, char** bufPtr, int* bufLenPtr);

    protected:
        ssize_t effRead(char* ptr);

        virtual int iclose();
        virtual ssize_t iread(char* buf, size_t count);
        virtual ssize_t iwrite(char* buf, size_t count);

        /**Sets the connection state to be blocking or non-blocking
           based on the type of the connection.
         */
        void setState();

        friend class TcpServerSocket;
        friend class TcpClientSocket;
        int _connId;
        bool _live;
        socklen_t _connAddrLen;
        struct sockaddr* _connAddr;
        bool _blocking;
        static bool _init;  /// Set to true if signal handler initialized
        bool _isClient; //  set if it is the client
};

#endif
