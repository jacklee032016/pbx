/*
 * $Id: TlsConnection.cxx,v 1.3 2007/03/03 18:38:56 lizhijie Exp $
 */


#include "TlsConnection.hxx"
#ifdef VOCAL_HAS_OPENSSL
 #include <openssl/err.h>
#endif
#include "cpLog.h"
#include "Mutex.hxx"
#include "Condition.hxx"

bool TlsConnection::initThreads = false;

extern "C"
{
    void my_locking_function(int mode, int n, const char *file, int line);
};




void my_locking_function(int mode, int n, const char *file, int line)
{
#ifdef VOCAL_HAS_OPENSSL
    static Vocal::Threads::Mutex myMutex[2000];

    if(mode & CRYPTO_LOCK)
    {
        myMutex[n].lock();
    }
    else
    {
        myMutex[n].unlock();
    }
#endif
}

//id_function()




static void setCallbacks()
{
#ifdef VOCAL_HAS_OPENSSL
    CRYPTO_set_locking_callback(my_locking_function);
#endif
}


TlsConnection::TlsConnection()
    : Connection(),
      ctx(0),
      ssl(0),
      meth(0),
      myCertificate(),
      myKey()
{
    if(!initThreads)
    {
        // set the callbacks
        setCallbacks();
        initThreads = true;
    }
}


TlsConnection::TlsConnection(Connection& other)
    : Connection(other),
      ctx(0),
      ssl(0),
      meth(0),
      myCertificate(),
      myKey()
{
    if(!initThreads)
    {
        // set the callbacks
        setCallbacks();
        initThreads = true;

    }
}


bool 
TlsConnection::isTls() const
{
    return ssl != 0;
}

int 
TlsConnection::iclose()
{
    if(ssl)
    {
#ifdef VOCAL_HAS_OPENSSL
	cpDebug(LOG_DEBUG_STACK, "closing SSL");
        SSL_shutdown(ssl);
	
	SSL_free (ssl);
	SSL_CTX_free (ctx);
#endif
    }
    cpDebug(LOG_DEBUG_STACK, "closing regular connection");

    return Connection::iclose();
}


const TlsConnection&
TlsConnection::operator=(const TlsConnection& x)
{
    if(&x != this)
    {
        this->Connection::operator=(x);
        ctx = x.ctx;
        ssl = x.ssl;
        meth = x.meth;
        myCertificate = x.myCertificate;
        myKey = x.myKey;
    }
    return *this;
}



ssize_t 
TlsConnection::iread(char* buf, size_t count)
{
    if(ssl)
    {
#ifdef VOCAL_HAS_OPENSSL
        while(1)
        {
            ssize_t t = SSL_read(ssl, buf, count);
            if(t == -1) 
            {
                ERR_print_errors_fp(stderr);
                int myerr = SSL_get_error(ssl, t);
                if(myerr == SSL_ERROR_WANT_READ)
                {
                    // select here to wait for the right bits but keep
                    // from looping relentlessly.  this only will work
                    // on UNIX.
                    int fd = SSL_get_fd(ssl);

                    if(fd >= 0)
                    {
                        fd_set rfds;
                        struct timeval tv;
                        int retval;
                        
                        FD_ZERO(&rfds);
                        FD_SET(fd, &rfds);
                        tv.tv_sec = 5;
                        tv.tv_usec = 0;
                        
                        retval = select(fd + 1, &rfds, NULL, NULL, &tv);
                        
                        if(retval <= 0)
                        {
                            errno = EIO;
                            return -1;
                        }
                    }
                    else
                    {
                        return -1;
                    }
                }
                else 
                {
                    return -1;
                }
            }
            else
            {
                return t;
            }
        }
#endif
    }
    return Connection::iread(buf, count);
}


ssize_t TlsConnection::iwrite(char* buf, size_t count)
{
    if(ssl)
    {
#ifdef VOCAL_HAS_OPENSSL
        ssize_t t = SSL_write(ssl, buf, count);
	if(t == -1) 
	{
	    ERR_print_errors_fp(stderr);
            int myerr = SSL_get_error(ssl, t);
            if(myerr == SSL_ERROR_WANT_READ || myerr == SSL_ERROR_WANT_WRITE)
            {
                return 0;
            }
	}
	return t;
#endif
    }
    return Connection::iwrite(buf, count);
}


int 
TlsConnection::initTlsClient()
{
#ifdef VOCAL_HAS_OPENSSL
    SSLeay_add_ssl_algorithms();
    meth = TLSv1_client_method();
    SSL_load_error_strings();
    ctx = SSL_CTX_new (meth);
    if(ctx == 0)
    {
	return -1;
    }

    ssl = SSL_new (ctx);
    if(ssl == 0)
    {
	return -1;
    }
    int err;

    err = SSL_set_fd (ssl, _connId);

    if(!err)
    {
        cpDebug(LOG_DEBUG, "failed to connect socket to TLS!");
        return -1;
    }


    while(1)
    {
        err = SSL_connect (ssl);

        if(err <= 0)
        {
            // check for ERROR_WANT_READ / ERROR_WANT_WRITE
            int myerr = SSL_get_error(ssl, err);
            
            if(myerr == SSL_ERROR_WANT_READ || myerr == SSL_ERROR_WANT_WRITE)
            {
                cpDebug(LOG_DEBUG, "try again!\n");
                vusleep(1000);
            }
            else
            {
                // issue
                cpDebug(LOG_DEBUG, "TLS error: %d ( %s )", err, ERR_error_string(SSL_get_error(ssl, err),0));
                break;
            }
        }
        else
        {
            break;
        }
    }

    return err;
#else
    return -1;
#endif
}


int 
TlsConnection::initTlsServer(const char* certificate,
                             const char* key)
{
#ifdef VOCAL_HAS_OPENSSL
    // check for TLSness

    char test[100];

    int bytes = ::recv(_connId, test, 32, MSG_PEEK);

    test[bytes-1] = '\0';

    if(strcmp(test, ".") == 0)
    {
        cpDebug(LOG_DEBUG_STACK, "TLS connection!\n");
    }

    char buf2[256];
    char* bptr = buf2;

    for(int i = 0; i < bytes ; ++i)
    {
        sprintf(bptr, "%2.2x", test[i]);
        bptr += 2;
    }
    cpDebug(LOG_DEBUG_STACK, "%s %d", buf2, bytes);

    myCertificate = certificate;
    myKey = key;
    
    cpDebug(LOG_DEBUG_STACK, "initalizing TLS server connection");

    SSLeay_add_ssl_algorithms();
    meth = TLSv1_server_method();
    SSL_load_error_strings();
    ctx = SSL_CTX_new (meth);
    if(ctx == 0)
    {
	cpLog(LOG_ERR, "no ctx");
	return -1;
    }

    // at this point, you need your certs.
    LocalScopeAllocator lo1;
    LocalScopeAllocator lo2;

    if (SSL_CTX_use_certificate_file(ctx, 
				     myCertificate.getData(lo1), 
				     SSL_FILETYPE_PEM) <= 0) 
    {
	ERR_print_errors_fp(stderr);
	cpLog(LOG_ERR, "failed to set certificate file %s",
	      myCertificate.getData(lo1));
	return -1;
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, 
				    myKey.getData(lo2), 
				    SSL_FILETYPE_PEM) <= 0) 
    {
	ERR_print_errors_fp(stderr);
	cpLog(LOG_ERR, "failed to set key file %s",
	      myKey.getData(lo1));
	return -1;
    }

    int err;
    assert(ctx != 0);
    ssl = SSL_new (ctx);
    if(ssl == 0)
    {
	cpLog(LOG_ERR, "failed to create new ssl");
	return -1;
    }
    assert(_connId >= 0);
    SSL_set_fd (ssl, _connId);

    while(1)
    {
        err = SSL_accept (ssl);
        if(err <= 0)
        {
            // check for ERROR_WANT_READ / ERROR_WANT_WRITE
            int myerr = SSL_get_error(ssl, err);
            
            if(myerr == SSL_ERROR_WANT_READ || myerr == SSL_ERROR_WANT_WRITE)
            {
                cpDebug(LOG_DEBUG, "try again!\n");
                vusleep(1000);
            }
            else
            {
                // issue
                cpDebug(LOG_DEBUG, "TLS error: %d ( %s )", err, ERR_error_string(SSL_get_error(ssl, err),0));
                break;
            }
        }
        else
        {
            break;
        }
    }
    return err;
#else
    return -1;
#endif
}


SSL* 
TlsConnection::getSsl()
{
    return ssl;
}


bool 
TlsConnection::hasTls()
{
#ifdef VOCAL_HAS_OPENSSL
    return true;
#else
    return false;
#endif
}


Data
TlsConnection::getErrMsg(int e)
{
    char tmp[1024];

#ifdef VOCAL_HAS_OPENSSL
    ERR_error_string_n(SSL_get_error(ssl, e), tmp, 1024);
#endif
    return Data(tmp);
}

