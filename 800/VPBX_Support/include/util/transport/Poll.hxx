#if !defined(ASSIST_POLL_HXX)
#define ASSIST_POLL_HXX
/*
* $Id: Poll.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include <map>
#include <vector>
#ifdef __APPLE__
#include "poll.h"
#else
#include <sys/poll.h>
#endif
#include "Sptr.hxx"
#include "Pipe.hxx"
#include "VMutex.h"
#include "Writer.hxx"
#include "ProtocolException.hxx"
#include "ConnectionBrokenException.hxx"
#include "SystemException.hxx"


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


using Assist::IO::FileDescriptor;


class Protocol;


/* Needed for map of pollfds.
 */
struct pollfdLess
{
    bool operator()(const pollfd & left, const pollfd & right) const;
};


/** A Poll wraps the operating system's poll() call.<br><br>
 *  
 *  The Poll and Protocol classes together form a type of Subject/Observer
 *  (see Design Patterns, Gamma, et al. page 293) pattern. The Poll object
 *  is the subject whereas the Protocol is the Observer. The Protocol object
 *  register's itself with the Poll. The protocol is interested in being 
 *  notified when it's file descriptor is active. Thus the observer provides 
 *  and an aspect to the server to reduce messaging activity. As well, since 
 *  the observer provides an aspect, the push model is used.
 *
 *  @see    Assist::Transport::Protocol
 *  @see    Assist::SystemException
 *  @see    Assist::Transport::ProtocolException
 */
class Poll : public Assist::IO::Writer
{

    public:


    	/** Default constructor
	 */
    	Poll();


    	/** Virtual destructor
	 */
    	virtual     	    	    ~Poll();


        /** When registering a file descriptor with a poll, the event type 
         *  determines which types of events you are interested in polling 
         *  the descriptor for.
         */
        typedef u_int16_t   EventType;
        

        /** You want to receive on the descriptor.
         */
        static EventType    Incoming;


        /** You want to send on the descriptor. Most useful with a 
         *  nonblocking descriptor.
         */
        static EventType    Outgoing;


        /** You want to receive priority messages on the descriptor.
         */
        static EventType    Priority;
        

    	/** Register the file descriptor for activity detection. 
         *  You can logically or the EventTypes to listen to different
         *  types of events. 
	 */	
	void	    	    registerFD(const FileDescriptor &, 
                                       EventType flag = Incoming,
                                       Protocol * = 0);


    	/** Update the flags for the file descriptor.
	 */	
    	void	    	    updateFD(const FileDescriptor &, 
                                     EventType flag = Incoming,
                                     Protocol * = 0);


    	/** No longer detect activity for protocol's file descriptor.
	 */	
	void	    	    unregisterFD(const FileDescriptor &);
        

    	/** Register the protocol's file descriptor for
    	 *  activity detection. 
	 */	
	void	    	    registerProtocol(Protocol &);


    	/** Update the flags for protocol's file descriptor for
    	 *  activity detection. 
	 */	
    	void	    	    updateProtocol(Protocol &);


    	/** No longer detect activity for protocol's file descriptor.
	 */	
	void	    	    unregisterProtocol(Protocol &);


    	/** A blocking call waiting for activity on the
    	 *  file descriptors of the registered sockets and protocols.
         *  This will block indefinately if the timeout is -1, or 
         *  for the number of milliseconds specified.
         *
    	 *  If a signal interrupts the poll, 0 is returned,
    	 *  otherwise the number of active file descriptors is
    	 *  returned.
	 */
    	int 	    	    poll(int timeout = -1)
		    	    	throw ( Assist::SystemException );


        /** See if the given file descritor is active.
         */
        EventType           fdActive(const FileDescriptor &) const;
        
        
    	/** Process the activity on the given protocols.
	 */
	void 	    	    processProtocols(int numberFdsActive)
    	    	    	    	throw ( Assist::Transport::ProtocolException );


    	/** Wake poll(). interrupt() should be called from a thread
    	 *  different from the thread that calls poll().
	 */		
	void	    	    interrupt()
		    	    	throw ( Assist::SystemException );


	/** Returns the number of outstanding interrupts pending on the 
	 *  poll, if any.
	 */
	u_int8_t    	    interruptCount() const;
		

    	/** Write the poll to an ostream.
	 */
	virtual ostream &           writeTo(ostream &) const;
					

    private:

    	void	    	    addInterruptorToFds();

	void	    	    addProtocolsToFds();

    	bool	    	    processInterruptor(pollfd &);

    	bool	    	    processPriority(pollfd &, Protocol *);

    	bool	    	    processIncoming(pollfd &, Protocol *);

    	bool	    	    processOutgoing(pollfd &, Protocol *);

    	bool	    	    processHangup(
		    	    	pollfd &, 
				Protocol *,
				ConnectionBrokenException * = 0);

    	bool	    	    processError(
		    	    	pollfd &, 
				Protocol *,
				SystemException * = 0);

     	typedef     map< pollfd, 
 		    	 Protocol *, 
 		    	 pollfdLess> 	ProtocolMap;

	ProtocolMap 	    myProtocols;
	bool	    	    myProtocolsChanged;

    	vector<pollfd>	    myFds;

    	IO::Pipe	    myInterruptor;
	u_int8_t   	    myInterruptCount;
	VMutex	    	    myInterruptorMutex;

    	// Suppress copying
	//
    	Poll(const Poll &);
	Poll & operator=(const Poll &);
};


} // namespace Transport
} // namespace Assist


#endif // !defined(ASSIST_POLL_HXX)
