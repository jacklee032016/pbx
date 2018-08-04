#if !defined(ASSIST_POLL_FIFO_SERVICE_HXX)
#define ASSIST_POLL_FIFO_SERVICE_HXX
/*
* $Id: PollFifoService.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "Service.hxx"
#include "PollFifo.hxx"
#include <string>


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


/** Infrastructure common to ASSIST to provide event based services.<br><br>
 */
namespace Services
{


/** Extends the Service class, using a PollFifo.<br><br>
 *
 *  @see    Assist::Services::Service
 *  @see    Assist::PollFifo
 *  @see    Assist::Transport::Protocol
 *  @see    Assist::SystemException
 *  @see    Assist::Transport::ProtocolException
 *  @see    Assist::Services::Event
 */
class PollFifoService : public Assist::Services::Service
{

    public:

    	
	/** Create the PollFifoServer an optional name.
	 */
    	PollFifoService(const char * name = 0);


    	/** Virtual destructor
	 */
	virtual     	    	~PollFifoService();


    	/** Accessor for PollFifo.
	 */
	FifoBase< Sptr<Event> >     	&   getFifo();


    	/** Accessor for PollFifo.
	 */
	const FifoBase< Sptr<Event> >	&   getFifo() const;


    	/** Register the protocol for activity detection.
	 */	
	void	    	registerProtocol(Protocol &);


    	/** Update the protocol's activity detection.
    	 */	
    	void	    	updateProtocol(Protocol &);


    	/** Register the protocol for activity detection.
	 */	
	void	    	unregisterProtocol(Protocol &);


    protected:

    	/** Handle an exception thrown by the fifo during wait. 
    	 *  The default behavior is to ignore the exception.
    	 *  Return true to shutdown, which will cause this thread to exit.
	 */
	virtual	void	onVException(VException &);


    	/** Handle a transport exception thrown by the fifo during
    	 *  wait. If we get this, poll is probably broken and other 
    	 *  threads probably can't send events to us, so we need
    	 *  to do a hard shutdown. Return true to shutdown, which
    	 *  will cause this thread to exit.
	 */
	virtual	void	onSystemException(SystemException &);


    	/** Handle a protocol exception thrown directly by a protocol
    	 *  or thrown indirectly by a MessageObserver using a
    	 *  protocol. The default action is to do nothing. Return true 
    	 *  to shutdown, which will cause this thread to exit.
	 */	
	virtual	void	onProtocolException(Transport::ProtocolException &);
		

	/** Calls the FifoBase's processMessages, then processes
    	 *  the protocols.
	 */
    	bool	    	processMessages(int numberFdsActive, ReturnCode &);
		

    private:

    	void	    	processProtocols(int numberFdsActive);

        PollFifo< Sptr<Event> >	    	myFifo;
};


} // namespace Services
} // namespace Assist


#endif // !defined(ASSIST_POLL_FIFO_SERVICE_HXX)
