#ifndef TcpServerSocket_hxx
#define TcpServerSocket_hxx
/*
* $Id: Tcp_ServerSocket.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include <sys/socket.h>
#include <netinet/in.h>

//User define class
#include "mstring.hxx"
#include "Connection.hxx"

class VNetworkException;
class TcpServerSocket;


/**
   Create a TCP server socket.<P>

   <b>Usage:</b>


   <pre>

   TcpServerSocket serverSocket(8000); // listen on port 8000

   while (serverSocket.getServerConn().isReadReady())
   {
       Connection conn;
       serverSocket.accept(conn);
       
       // now conn can be read / written (see Connection for details)
       // to get the actual data.

   }
   </pre>

*/

class TcpServerSocket
{
    public:

	/**
	   Create a TCP server socket.

	   @param servPort   listen on this port.
	   @throw VNetworkException
	*/
        TcpServerSocket(int servPort) throw (VNetworkException&);

	/**
	   Create a TCP server socket.

	   by default, it uses Venv::VPS_PORT.

	   @throw VNetworkException
	*/
        TcpServerSocket() throw (VNetworkException&);
        TcpServerSocket(const TcpServerSocket&);

        TcpServerSocket& operator=(TcpServerSocket& other);
        ~TcpServerSocket();


	/**
	   Accept an incoming connection, and create a Connection
	   object for it.
	   
	   @param con   the client connection is set to this object.
	   @throw VNetworkException
	*/
        int accept(Connection& con) throw (VNetworkException&);

	/**
	   close the server connection.
	*/
        void close();

	/// get the client connection (what is this?)
        inline Connection& getClientConn()
        {
            return _clientConn;
        };

	/// get the server connection
        inline Connection& getServerConn()
        {
            return _serverConn;
        };
    private:
        ///
        void listenOn(int servPort) throw (VNetworkException&);

        Connection _serverConn;
        Connection _clientConn;
};

#endif
