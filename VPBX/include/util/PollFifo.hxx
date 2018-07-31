#if !defined(ASSIST_POLL_FIFO_HXX)
#define ASSIST_POLL_FIFO_HXX
/*
* $Id: PollFifo.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "FifoBase.h"
#include "Poll.hxx"
#include "SystemException.hxx"
#include "SystemStatus.hxx"
#include "VLog.hxx"


namespace Assist {
    namespace Transport {
    	class Protocol;
    	class ProtocolException;
    }
}


/** Infrastructure common to ASSIST.
 */
namespace Assist 
{


class SystemException;
class SystemStatus;


using Assist::Transport::Protocol;
using Assist::Transport::Poll;


/** This is where is Fifo code meets the transport code.<br><br>
 *
 *  The mechanism to block is the Poll. Protocols can also register with 
 *  this Fifo. When an incoming message arrives, wait returns.
 *
 *  @see    FifoBase
 *  @see    Fifo
 */
template<class Msg>
class PollFifo : public FifoBase<Msg>
{
    public:

	/** Create an empty fifo.
	 */
	PollFifo();
				    

	/** Delete the fifo. Must be empty to delete.
	 */
	virtual     ~PollFifo();


    	/** Register the protocol for activity detection.
	 */
	void        registerProtocol(Protocol &);


    	/** Update the protocol's activity detection.
	 */
	void        updateProtocol(Protocol &);


    	/** Register the protocol for activity detection.
	 */
	void        unregisterProtocol(Protocol &);


    	/** Process the activity on the given protocols.
	 */
	void        processProtocols(int numberFdsActive)
		    	throw ( Assist::Transport::ProtocolException );


        /** Poll accessor.
         */
        Poll &      poll();
        

        /** Const Poll accessor.
         */
        const Poll &    poll() const;
        
        
    private:

    	/** Wake the blocked thread.
	 */
    	virtual void	    wakeup() throw ( VException );


    	/** Wait until a message has arrived, or there is polled io
    	 *  activity. If there is polled io activity, the return
    	 *  value is the number of file descriptors active, otherwise
    	 *  0 is returned. The fifo must be empty to detect polled io
    	 *  activity.
    	 *  
    	 *  Note that the mutex is locked upon entry. It is the responsibility
    	 *  of the user to unlock the mutex if necessary, making sure
    	 *  to relock it before exit.
	 */
      	int         wait(milliseconds_t relativeTimeout)
		        throw ( VException );


    	Poll	    myPoll;
};


#include "PollFifo.cc"


} // namespace Assist


#endif // !defined(ASSIST_POLL_FIFO_HXX)
