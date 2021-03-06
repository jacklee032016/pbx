#if !defined(VOCAL_CONTEXT_HXX)
#define VOCAL_CONTEXT_HXX
/*
 * $Id: Context.hxx,v 1.2 2007/03/01 20:04:29 lizhijie Exp $
 */

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
class StateInterface;


/** The basis of a finite state machine.<br><br>
 *
 *  The context is responsible for maintaining the current state of the
 *  finite state machine, and provide and means to set and verify the
 *  starting and stopping states of that machine. Users of finite
 *  state machines will typically only interface with the finite state
 *  machine class that extends this class.
 *
 *  @see    Vocal::Behavioral::StateInterface
 *  @see    Vocal::Behavioral::Stimulus
 */
class Context : public Vocal::IO::Writer
{

    protected:

    	/** Default constructor
	 */
    	Context();

    public:


    	/** Virtual destructor
	 */
	virtual     ~Context();


    	/** Reset the current state back to the starting state.
	 */
    	void	    	        reset() throw();


    	/** Reset the current state to the given state.
	 */
    	void	    	    	reset(StateInterface &) throw();


    	/** Set the starting state to the given state.
	 */
	void	    	    	startState(StateInterface &);


    	/** Query the context whether the current state is the starting state.
	 */
	bool	    	    	inStartState() const;


    	/** Set the stopping state to the given state.
	 */
	void	    	    	stopState(StateInterface &);


    	/** Query the context whether the current state is the stopping state.
	 */
	bool	    	    	inStopState() const;


    	/** Pass a stimulus to the current state.
	 */
    	void        	    	stimulate(const Stimulus &) throw ();


    	/** Write the context to am ostream.
	 */
	virtual ostream &       writeTo(ostream &) const;


    	/** The name of this class of context.
	 */
    	virtual const char *	name() const = 0;


    	/** Query the context whether the current state is the same as the 
	 *  given state.
	 */
    	bool	    	verifyState(const StateInterface &);


    private:
    
    	StateInterface	    *	myState;
	StateInterface	    *	myStartState;
	StateInterface	    *	myStopState;
};


} // namespace Behavioral
} // namespace Vocal


#endif // !defined(VOCAL_CONTEXT_HXX)
