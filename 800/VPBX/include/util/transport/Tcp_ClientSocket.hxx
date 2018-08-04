#ifndef TcpClientSocket_hxx
#define TcpClientSocket_hxx
/*
* $Id: Tcp_ClientSocket.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

//User define class
#include "Connection.hxx"
#include "VNetworkException.hxx"
#include "Data.hxx"

class NetworkAddress;

/**
   Create A client socket for TCP.<p>

   <b>Usage:</b>

   <pre>
   NetworkAddress serverAddress;
   TcpClientSocket mySocket(serverAddress, true, false);  // nonblocking

   mySocket.connect(); // connect

   Connection& conn = mySocket.getConn();

   // now, you can use connection to read or write to the server.  see
   // Connection class for details.

   </pre>
*/
class TcpClientSocket
{
    public:

        /**
           Create client TCP connection.

           @param hostName   host:port to connect to.  Host can be fqdn or ip addr as string.
           @param closeCon   allow close() to close the connection.
           @param blocking   reads should be blocking.

        */
        TcpClientSocket(const string& hostName,
                        bool closeCon = true, bool blocking = true);

        /**
           Create client TCP connection.

           @param hostName   host name to connect to.  Host can be fqdn or ip addr as string.
           @param port       host port to connect to.
           @param closeCon   allow close() to close the connection.
           @param blocking   reads should be blocking.

        */
        TcpClientSocket(const string& hostName, int servPort,
                        bool closeCon = true, bool blocking = true);

        /**
           Create client TCP connection.

           @param server    host and port to connect to.
           @param closeCon   allow close() to close the connection.
           @param blocking   reads should be blocking.

        */
        TcpClientSocket(const NetworkAddress& server,
                        bool closeCon = true, bool blocking = true);

        /**
           Create client TCP connection.

           This is a copy constructor.
        */
        TcpClientSocket(const TcpClientSocket&);

        /// assignment operator
        TcpClientSocket& operator=(TcpClientSocket& other);


        ~TcpClientSocket();


        /**
           connect to the far side.
        */
        void connect() throw (VNetworkException&);
        /**
           close the connnection.
        */
        void close();

        /**
           get the Connection which was created by this object.
        */
        inline Connection& getConn()
        {
            return _conn;
        };
    private:
        void initalize();
        const char* connectionDesc(struct addrinfo* laddr, char* descBuf, int bufLen) const;


        Connection _conn;
        Data _hostName;
        int _serverPort;
        bool _closeCon;
        bool _blocking;
};

#endif
