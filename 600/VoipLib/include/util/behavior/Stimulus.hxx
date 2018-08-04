#if !defined(VOCAL_STIMULUS_HXX)
#define VOCAL_STIMULUS_HXX
/*
 * $Id: Stimulus.hxx,v 1.2 2007/03/01 20:04:29 lizhijie Exp $
 */

#include "Event.hxx"
#include "Sptr.hxx"
#include "Writer.hxx"
#include <string>


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Infrastructure in VOCAL related to "algorithms and the assignment of
 *  responsibilities between objects".<br><br>
 *
 *  See Design Patterns, ISBN 0-201-63361-2, Chapter 5.
 */
namespace Behavioral
{


/** A stimulus is some type of event (perhaps even an Event) that
 *  can drive a finite state machine.<br><br>
 *
 *  @see    Vocal::Behavioral::Context
 *  @see    Vocal::Behavioral::StateInterface
 *  @see    Event
 */
class Stimulus : public Vocal::IO::Writer
{

    public:


    	/** Create a stimulus, given the id and an optional name. 
	 */
        Stimulus(
	    u_int32_t 	    id, 
	    const char *    name = 0
	);
			

    	/** Copy constructor
	 */
    	Stimulus(const Stimulus &);


    	/** Virtual destructor
	 */
	virtual     	~Stimulus();


    	/** Assignment operator
	 */
    	Stimulus &  	operator=(const Stimulus &);


    	/** Equality relational operator
	 */
    	bool	    	operator==(const Stimulus &) const;


    	/** Less than relational operator
	 */
	bool	    	operator<(const Stimulus &) const;


    	/** Allows an Event to be associated with this Stimulus.
	 */
    	void	    	event(Sptr<Vocal::Signaling::Event>);


    	/** Event accessor, may be 0 if not Event is associated.
	 */
    	Sptr<Vocal::Signaling::Event> 	event() const;


    	/** Write the Stimulus to an ostream.
	 */
	ostream &       writeTo(ostream &) const;


    private:

    	u_int32_t   	    	    	    myId;
	string	    	    	    	    myName;
	Sptr<Vocal::Signaling::Event> 	    myEvent;
};


} // namespace Behavioral
} // namespace Vocal


#endif // !defined(VOCAL_STIMULUS_HXX)
