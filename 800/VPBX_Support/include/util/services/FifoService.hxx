#if !defined(ASSIST_FIFO_SERVICE_HXX)
#define ASSIST_FIFO_SERVICE_HXX
/*
* $Id: FifoService.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "Service.hxx"
#include "Fifo.h"


/** Infrastructure common to ASSIST.
 */
namespace Assist 
{


/** Infrastructure common to ASSIST to provide event based services.<br><br>
 */
namespace Services
{


/** A service based on the Fifo.<br><br>
 *
 *  This only responds to events on the Fifo.
 *
 *  @see    Assist::Services::Service
 *  @see    Fifo
 *  @see    FifoBase
 */
class FifoService : public Assist::Services::Service
{
    public:
    
	/** Create the service given an optional name.
	 */
    	FifoService(const char * name = 0);


	/** Virtual destructor
	 */
	virtual     	    	~FifoService();


    	/** Accessor for PollFifo.
	 */
	FifoBase< Sptr<Event> >     	&   getFifo();


    	/** Accessor for PollFifo.
	 */
	const FifoBase< Sptr<Event> >	&   getFifo() const;

    private:

        Fifo< Sptr<Event> >	    	    myFifo;
};


} // namespace Services
} // namespace Assist


#endif // !defined(ASSIST_FIFO_SERVICE_HXX)
