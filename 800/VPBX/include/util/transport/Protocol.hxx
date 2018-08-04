#if !defined(ASSIST_PROTOCOL_HXX)
#define ASSIST_PROTOCOL_HXX
/*
* $Id: Protocol.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "NonCopyable.hxx"
#include "Writer.hxx"
#include "ProtocolException.hxx"
#include "SystemException.hxx"
#include <string>


// Creating a forward reference here due to doc++ bug.
//
namespace Assist {
    namespace IO {
    	class FileDescriptor;
    }
}


/** Infrastructure common to ASSIST.
 */
namespace Assist 
{


/** Infrastructure in ASSIST related to making network transport layer 
 *  connections.<br><br>
 */
namespace Transport 
{


class ConnectionBrokenException;
class Poll;


/** A Protocol is the object that gives meaning to the bytes
 *  available from a FileDescriptor.<br><br>
 *  
 *  The Poll and Protocol classes together form a type of Subject/Observer
 *  (see Design Patterns, Gamma, et al. page 293) pattern. The Poll object
 *  is the subject whereas the Protocol is the Observer. The Protocol object
 *  register's itself with the Poll. The protocol is interested in being 
 *  notified when it's file descriptor is active. Thus the observer provides 
 *  and an aspect to the server to reduce messaging activity. As well, since 
 *  the observer provides an aspect, the push model is used.
 *
 *  @see    Assist::Transport::Poll
 *  @see    Assist::Signaling::PollFifo
 *  @see    Assist::IO::FileDescriptor
 *  @see    Assist::ConnectionBrokenException
 */
class Protocol : public Assist::IO::Writer, public Assist::NonCopyable
{
    public:

    	/** Create with an optional name.
	 */
    	Protocol(const char * name = 0);


    	/** Create with an optional name.
	 */
    	Protocol(Poll & poll, const char * name = 0);


    	/** Virtual destructor
	 */
    	virtual ~Protocol();
	

    	/** FileDescriptor accessor.
	 */
	virtual const IO::FileDescriptor &  getFileDescriptor() const = 0;
	

    	/** Callback that notifies the user of information available on the
	 *  associated FileDescriptor. The default behavior is to do 
	 *  nothing, thus this should be overridden by the user.
	 */
	virtual void	onIncomingAvailable();


    	/** Callback that notifies the user of the ability to send information 
	 *  on the associated FileDescriptor. The default behavior is to do 
	 *  nothing, thus this should be overridden by the user.
	 */
	virtual void	onOutgoingAvailable();


    	/** Callback that notifies the user of priority information available 
	 *  on the associated FileDescriptor. The default behavior is to do 
	 *  nothing, thus this should be overridden by the user.
	 */
	virtual void	onPriority();


    	/** Callback that notifies the user that the associated FileDescriptor
	 *  has disconnected. If a ConnectionBrokenException had been caught,
	 *  it is passed along. The default behavior is to do throw
	 *  a HANGUP_EXCEPTION.
	 */
	virtual void	onDisconnect(ConnectionBrokenException * = 0)
    	    	    	    throw ( Assist::Transport::ProtocolException );


    	/** Callback that notifies the user that an error has been detected 
	 *  on the associated FileDescriptor. If a SystemException had 
	 *  been caught, it is passed along. The default behavior is to 
	 *  do throw an ERROR_EXCEPTION.
	 */
	virtual void	onError(SystemException * = 0)
    	    	    	    throw ( Assist::Transport::ProtocolException );


    	/** If the poll receives an unknown exception, it calls this method,
	 *  which by default rethrows the exception. 
	 */
    	virtual void	handleUnknownException();
					    

    	/** Allow the Protocol to be notified of incoming data.
	 */
    	void	    	awaitIncoming(bool);


    	/** Query the Protocol state to see if it is to be notified of 
	 *  incoming data.
	 */
	bool	    	awaitIncoming() const;


    	/** Allow the Protocol to be notified of outgoing availability.
	 */
    	void	    	awaitOutgoing(bool);


    	/** Query the Protocol state to see if it is to be notified of 
	 *  outgoing availability.
	 */
    	bool	    	awaitOutgoing() const;


    	/** Allow the Protocol to be notified of priority messages.
	 */
    	void	    	awaitPriority(bool);


    	/** Query the Protocol state to see if it is to be notified of 
	 *  priority messages.
	 */
    	bool	    	awaitPriority() const;


        /**
         */
        const string &  name() const;
        
        
    	/** Write the protocol to an ostream.
	 */
	virtual ostream &   writeTo(ostream &) const;


    	/** Equality operator.
	 */
    	bool	    	operator==(const Protocol &);


    	/** Less than relational operator.
	 */
    	bool	    	operator<(const Protocol &);
		

    private:
		bool	    	    	    myIncoming;
    	    	bool	    	    	    myOutgoing;
		bool	    	    	    myPriority;
		string      	    	    myName;
                Poll                    *   myPoll;
};


} // namespace Transport
} // namespace Assist


#endif // !defined(ASSIST_PROTOCOL_HXX)
