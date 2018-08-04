#if !defined(VOCAL_ACTION_HXX)
#define VOCAL_ACTION_HXX

/*
 * $Id: Action.hxx,v 1.2 2007/03/01 20:04:29 lizhijie Exp $
 */

#include "VocalCommon.hxx"
#include "Writer.hxx"


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


class Stimulus;


/** An Action object is used to implement an algorithm.<br><br>
 *
 *  Actions can be chained one after another.<br><br>
 *
 *  An example use of an Action can be seen in StateInterface class.
 *  The StateInterface, rather than implementing each response to 
 *  a Stimulus with a method, associates an Action with each Stimulus.
 *  The Action is a more discrete element than a state, and therefore
 *  Actions can be shared among many StateInterfaces or perhaps many
 *  state machines.
 *  <br><br>
 *
 *  @see    Vocal::Behavioral::StateInterface
 *  @see    Vocal::Behavioral::Stimulus
 *  @see    Vocal::IO::Writer
 *  @see    Vocal::ReturnCode
 *
 */
class Action : public Vocal::IO::Writer
{


    public:


    	/** Default constructor. By default, no actions are chained.
	 */
    	Action();
				

	/** Virtual destructor, since you should be extending this class.
	 */
	virtual ~Action();


    	/** The public interface to all Actions are the action() methods.
	 *  This method call's this classes onAction method, and then if 
	 *  another Action is chained to this one, calls the action method 
	 *  on the chained Action. This variation allows an action() to be
	 *  called without a Stimulus.
	 */
    	ReturnCode  	    	action() throw ();


    	/** The public interface to all Actions are the action() methods.
	 *  This method call's this classes onAction method, and then if 
	 *  another Action is chained to this one, calls the action method 
	 *  on the chained Action. This action to be
	 *  called without a Stimulus.
	 */
    	ReturnCode  	    	action(const Stimulus &) throw ();


    	/** Creates an association between this Action and the chained Action,
	 *  such that the chained Action will be done after this one.
	 */
    	virtual void	    	chain(Action &);
	

    	/** Enabled writing this Action to an ostream. By default writes the
	 *  name().
	 */
	virtual ostream &   	writeTo(ostream &) const;
	

	/** The name of the Action. Useful for logging.
	 */
    	virtual const char *	name() const;


    protected:


    	/** Method to be extended by the user of this class. Used by action().
	 */
    	virtual ReturnCode  	onAction(const Stimulus * = 0) throw();


    	// Used for chaining.
	//
    	Action	    	    *	myNextAction;
};


} // namespace Behavioral
} // namespace Vocal


#endif // !defined(VOCAL_ACTION_HXX)
