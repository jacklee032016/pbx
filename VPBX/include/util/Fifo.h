#if !defined(ASSIST_FIFO_HXX)
#define ASSIST_FIFO_HXX
/*
* $Id: Fifo.h,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "FifoBase.h"
#include "Condition.hxx"
#include <cerrno>


/* Infrastructure common to ASSIST.
 */
namespace Assist 
{


/** 
    This template class implements a thread safe FIFO (first in first
    out) queue of messages.  @see FifoBase# for its methods.

 */
template <class Msg> class Fifo : public FifoBase<Msg>
{
    public:


        /** Create an empty FIFO
         */
        Fifo();


        /** Delete the FIFO - must be empty when deleted
         */
        virtual ~Fifo();

    private:


        /** Wake's up the blocked thread.
         */
        virtual void	wakeup()
        throw ( VException );


        /** Blocks the thread, waiting for a message. Note that the
         *  mutex_ is locked upon entry. It is the responsibility
         *  of the user to unlock the mutex if necessary.
         */
        virtual int	wait(milliseconds_t relativeTime)
        throw ( VException );


        /** Conditional variable for waiting until the FIFO is not empty
         */
        Condition       myCondition;
};


#include "Fifo.cc"


} // namespace Assist


#endif // !defined(ASSIST_FIFO_HXX)
