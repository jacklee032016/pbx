#if !defined(SOCKET_TYPE_DOT_H)
#define SOCKET_TYPE_DOT_H
/*
* $Id: SocketType.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "Writer.hxx"

#if defined(__FreeBSD__) || defined(__APPLE__)
#include <sys/types.h>
#endif

#include <sys/socket.h>
#include <string>


/** Infrastructure common to ASSIST.
 */
namespace Assist 
{


/** Infrastructure in ASSIST related to making network transport layer 
 *  connections.<br><br>
 */
namespace Transport 
{


using std::string;


/** Wrapper for SOCK_* types defined in sys/socket.h.<br><br>
 *
 *  @see    Assist::Transport::Socket
 */
class SocketType : public Assist::IO::Writer
{
    public:

    	/** Construct by sys/socket.h SOCK_* manifest constant.
	 */
	SocketType(int type = SOCK_DGRAM);


    	/** Copy constructor.
	 */
	SocketType(const SocketType &);
	

    	/** Virtual destructor.
	 */
	virtual ~SocketType();


    	/** Assignment operator
	 */
    	SocketType &	    operator=(const SocketType &);
	

    	/** Accessor
	 */
	int  	    	    operator()();
	

    	/** Mutator
	 */
    	void	    	    operator()(int);


    	/** Equality relational operator
	 */
	bool	    	    operator==(const SocketType &) const;


    	/** Inequality relational operator
	 */
	bool	    	    operator!=(const SocketType &) const;


    	/** Less than relational operator
	 */
	bool	    	    operator<(const SocketType &) const;


    	/** Less than or equal relational operator
	 */
	bool	    	    operator<=(const SocketType &) const;


    	/** Greater than relational operator
	 */
	bool	    	    operator>(const SocketType &) const;


    	/** Greater than or equal to relational operator
	 */
	bool	    	    operator>=(const SocketType &) const;

        
        /**
         */
        static const string &      name(int);
        

    	/** Write SocketType to an ostream.
	 */
	ostream &           writeTo(ostream &) const;
	

    private:

        static const string     tcp;
        static const string     udp;
        static const string     unknown;
            	
    	int  	    	    type_;
};


} // namespace Transport
} // namespace Vovida


#endif // !defined(SOCKET_TYPE_DOT_H)
