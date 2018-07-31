#if !defined(ASSIST_EVENT_HXX)
#define ASSIST_EVENT_HXX
/*
* $Id: Event.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "Writer.hxx"
#include "NonCopyable.hxx"


/** Infrastructure common to ASSIST.<br><br>
 */
namespace Assist 
{


/** Infrastructure common to ASSIST to provide events based services.<br><br>
 */
namespace Services
{


/** Common base class for events passed between threads.
 */
class Event : public Assist::IO::Writer, public Assist::NonCopyable
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
} // namespace Assist


#include "DeprecatedEvent.hxx"


#endif // !defined(ASSIST_EVENT_HXX)
