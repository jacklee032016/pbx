#if !defined(ASSIST_STATEINTERFACE_HXX)
#define ASSIST_STATEINTERFACE_HXX
/*
* $Id: StateInterface.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include <map>
#include <string>
#include "Writer.hxx"
#include "Stimulus.hxx"
#include "NonCopyable.hxx"


/** Infrastructure common to ASSIST.
 */
namespace Assist 
{


/** Infrastructure in ASSIST related to "algorithms and the assignment of
 *  responsibilities between objects".<br><br>
 *
 *  See Design Patterns, ISBN 0-201-63361-2, Chapter 5.
 */
namespace Behavioral
{


class Action;


/** A concrete class representing a state in a finite state machine.<br><br>
 *
 *  The state allows an entry Action to be specified such that upon entry
 *  into the state, the entry action will be executed. The state also allows
 *  an exit action to be specified such that upon leaving the state, the
 *  exit action will be executed.<br><br>
 *
 *  A state will respond to a given Stimulus by performing a series of Actions
 *
 *
 *
 *  @see    Assist::Behavioral::Stimulus
 *  @see    Assist::Behavioral::Context
 *  @see    Assist::Behavioral::Action
 */
class StateInterface : public Assist::IO::Writer, public Assist::NonCopyable
{

    public:


    	/** Contruct the state, given an optional name, given an optional
	 *  entry action, and given an optional exit action.
	 */
        StateInterface(
	    const char 	    * 	name	= 0,
	    Action  	    *	entry 	= 0,
	    Action  	    *	exit 	= 0
	);


    	/** Virtual destructor.
	 */
	virtual     ~StateInterface();


    	/** For the given Stimulus, the Action will be executed, and the
	 *  state machine will transition to the nextState.
	 */
	void		    addAction(
	    	    		const Stimulus 	    &	stimulus,
				Action  	    &	action,
	    	    		StateInterface      &	nextState
			    );


    	/** For the given Stimulus, the Action will be executed. If the
	 *  Action was successful, it will transition to the nextState, 
	 *  otherwise it will transition to the nextStateOnFail.
	 *
	 *  This is used only for conditional state transitions, where
	 *  an action may fail depending on external data associated
	 *  with the stimulus.
	 */
	void		    addAction(
	    	    		const Stimulus 	    &	stimulus,
				Action  	    &	action,
	    	    		StateInterface      &	nextState,
				StateInterface	    &	nextStateOnFail
			    );


    	/** Associate an entry action to be performed.
	 */
    	void        	    entryAction(Action &);


    	/** Associate an exit action to be performed.
	 */
    	void        	    exitAction(Action &);


    	/** Called when entering this state.
	 */
    	virtual void        enter() throw ();


    	/** Called to handle the given stimulus.
	 */
    	StateInterface &    stimulate(
	    	    	    	const Stimulus  &   stimulus
			    ) throw ();


    	/** Called when exiting the state.
	 */
	virtual void	    exit() throw();


    	/** Write the state to an ostream.
	 */
	virtual ostream &   writeTo(ostream &) const;


    	/** Returns the instance specific name, if given during construction.
	 */
    	virtual const string &	    name() const;


    private:

    	class	StimulusData
	{
	    public:
	    	    StimulusData(
			Action  	    &	action,
	    	    	StateInterface      &	nextState,
    	    	    	StateInterface	    *	nextStateOnFail
		    );

    	    	Action  	    &	action_;
    	    	StateInterface      &	nextState_;
    	    	StateInterface      *	nextStateOnFail_;
	};
	
    	typedef map<Stimulus, StimulusData>    StimulusTable;
    	typedef pair<Stimulus, StimulusData>   StimulusPair;
	
	StimulusTable  	    	stimulusTable_;

    	string	    	    	myName;
    	Action	    	    *	myEntry;
    	Action	    	    *	myExit;
};


} // namespace Behavioral
} // namespace Assist


#endif // !defined(ASSIST_STATEINTERFACE_HXX)
