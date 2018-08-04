#if !defined(VOCAL_FIFO_SERVICE_HXX)
#define VOCAL_FIFO_SERVICE_HXX
/*
 * $Id: FifoService.hxx,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */

#include "Service.hxx"
#include "Fifo.h"


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Infrastructure common to VOCAL to provide event based services.<br><br>
 */
namespace Services
{


/** A service based on the Fifo.<br><br>
 *
 *  This only responds to events on the Fifo.
 *
 *  @see    Vocal::Services::Service
 *  @see    Fifo
 *  @see    FifoBase
 */
class FifoService : public Vocal::Services::Service
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
} // namespace Vocal


#endif // !defined(VOCAL_FIFO_SERVICE_HXX)
