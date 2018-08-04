#ifndef TLSCONNECTION_HXX_
#define TLSCONNECTION_HXX_
/*
* $Id: TlsConnection.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "Connection.hxx"

#ifdef ASSIST_HAS_OPENSSL
#include <openssl/ssl.h>
#endif

#include "Data.hxx"

#ifndef ASSIST_HAS_OPENSSL
typedef void SSL;
typedef void SSL_CTX;
typedef void SSL_METHOD;
#endif


/**
   A TLS based (rather than TCP-based) Connection object.  Network
   connections are represented by these objects.<p>

   <b>Client Usage</b><p>

   <pre>

   // we assume here that an open TCP Connection exists in conn

   TlsConnection&#42; myConnection = 0;

   if(TlsConnection::hasTls() && myUseTls)
   {
     // do the TLS stuff if you need to.
   
     myConnection = new TlsConnection(&#42;conn);
     int result = myConnection->initTlsClient();
     if(result > 0)
     {
       
       // at this point, TLS is ok, so conn gets silently replaced
       // with the TLS form of the connection.
       
       conn = myConnection;
     }
     else
     {
       // here, the TLS negotiation failed, so print an error.
       LocalScopeAllocator lo;
       Data tmp = myConnection->getErrMsg(-1);
       cpLog(LOG_DEBUG, "TLS failed: %s!", tmp.getData(lo));
     }

     // from here, you can use the regular Connection procedure.
     
   </pre>

   Please see provisioning/psLib/PSNetworkClient.cxx for details.<p>

   <b>Server Usage</b><p>

   we again assume that you have created a TcpServerSocket already.
   Instead of passing a regular Connection in to the accept(), you
   should pass a TlsConnection.  Assume that there is already a
   TcpServerSocket <i>sock</i> which exists:

   <pre>
     TlsConnection conn;
     sock.accept(conn);

     // try to do TLS negotiation
     // the certificate and key file need to be somewhere
     if(conn.initTlsServer("path/to/certs", "path/to/keys") <= 0)
     {
       cpLog(LOG_ERR, "failed to do TLS correctly");
     }
     else
     {
       // everything OK, so continue.
     }


*/
class TlsConnection : public Connection
{
    public:
        /// initiate an empty TLS server connection.
        TlsConnection();

        /** 
            Begin a TLS client connection over an existing, open, TCP
            connection.
         */
        TlsConnection(Connection& other);

        /// initialize as a TLS client
        int initTlsClient();

        /// initialize as a TLS server
        int initTlsServer(const char* certificate,
                          const char* key);

        /// get the SSL* object (from openssl)
        SSL* getSsl();

        /**
           Does this connection have TLS running at this moment.
           @return true if TLS active on this connection, false otherwise.
        */
        bool isTls() const;

        /// assignment operator
        const TlsConnection& operator=(const TlsConnection& x);

        /** 
            Tell whether this object was built with TLS support.

            @return If not, return false.  if so, return true.
        */
        static bool hasTls();

        /** return the openssl error message.  this function be called
         * from the same thread as received the error.
         *
         * @param e    the error code which was returned by the call.
         */
        Data getErrMsg(int e);

    protected:
        virtual int iclose();
        virtual ssize_t iread(char* buf, size_t count);
        virtual ssize_t iwrite(char* buf, size_t count);

        SSL_CTX* ctx;
        SSL*     ssl;
        SSL_METHOD *meth;
        Connection* myConn;
        Data myCertificate;
        Data myKey;

        static bool initThreads;
};

#endif
