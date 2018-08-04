#if !defined(VOCAL_EVENT_HXX)
#define VOCAL_EVENT_HXX
/*
 * $Id: Event.hxx,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */


#include "Writer.hxx"
#include "NonCopyable.hxx"


/** Infrastructure common to VOCAL.<br><br>
 */
namespace Vocal 
{


/** Infrastructure common to VOCAL to provide events based services.<br><br>
 */
namespace Services
{


/** Common base class for events passed between threads.
 */
class Event : public Vocal::IO::Writer, public Vocal::NonCopyable
{
    public:

        Event();

        virtual ~Event();


        /** Provide the name of the class of event.
         */
        virtual const char*  name() const = 0;


        /** Write the event to a stream.
         */
        virtual ostream & writeTo(ostream &) const;
};


} // namespace Services
} // namespace Vocal


#include "DeprecatedEvent.hxx"


#endif // !defined(VOCAL_EVENT_HXX)
