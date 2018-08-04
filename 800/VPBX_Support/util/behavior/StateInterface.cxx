/*
* $Id: StateInterface.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include <cassert>
#include "StateInterface.hxx"
#include "Stimulus.hxx"
#include "Action.hxx"
#include "VLog.hxx"


using Assist::Behavioral::StateInterface;
using Assist::Behavioral::Stimulus;
using Assist::Behavioral::Action;
using Assist::Logging::VLog;
using Assist::ReturnCode;


StateInterface::StateInterface(
    const char      *	name,
    Action  	    *	onEntry,
    Action  	    *	onExit
)
    :	myName(name ? name : "NullState"),
    	myEntry(onEntry),
    	myExit(onExit)
{
    const string    fn("StateInterface::StateInterface");
    VLog    	    log(fn);
    
    VDEBUG(log) << fn << ": " << *this << VDEBUG_END(log);
}


StateInterface::~StateInterface()
{
    const string    fn("StateInterface::~StateInterface");
    VLog    	    log(fn);
    
    VDEBUG(log) << fn << ": " << myName << VDEBUG_END(log);
}


void		    
StateInterface::addAction(
    const Stimulus 	    &	stimulus,
    Action  	    	    &	action,
    StateInterface          &	nextState
)
{
    const string    fn("StateInterface::addAction");
    VLog    	    log(fn);

    bool rc =	stimulusTable_.insert(
    	    	    StimulusPair(stimulus, StimulusData(action, nextState, 0))
	      	).second;

    assert( rc );

    VDEBUG(log) << fn 
    	    	<< "\n\tState          :\t" << myName
    	    	<< "\n\tStimulus       :\t" << stimulus
	    	<< "\n\tAction         :\t" << action
	    	<< "\n\tNextState      :\t" << nextState.name()
		<< VDEBUG_END(log);
}


void		    
StateInterface::addAction(
    const Stimulus 	    &	stimulus,
    Action  	    	    &	action,
    StateInterface          &	nextState,
    StateInterface          &	nextStateOnFail
)
{
    const string    fn("StateInterface::addAction");
    VLog    	    log(fn);

    bool rc =	stimulusTable_.insert(
    	    	    StimulusPair(stimulus, StimulusData(action, nextState, &nextStateOnFail))
	      	).second;

    assert( rc );

    VDEBUG(log) << fn 
    	    	<< "\n\tState          :\t" << myName
    	    	<< "\n\tStimulus       :\t" << stimulus
	    	<< "\n\tAction         :\t" << action
	    	<< "\n\tNextState      :\t" << nextState.name()
	    	<< "\n\tNextStateOnFail:\t" << nextStateOnFail.name()
		<< VDEBUG_END(log);
}


void        	    
StateInterface::entryAction(Action & onEntry)
{
    myEntry = &onEntry;
}


void        	    
StateInterface::exitAction(Action & onExit)
{
    myExit = &onExit;
}


void        	    
StateInterface::enter() throw ()
{
    const string    fn("StateInterface::enter");
    VLog    	    log(fn);

    if ( myEntry )
    {
    	VDEBUG(log) << fn << ": IsdnState: " << myName
	    	    << ": Action: " << *myEntry << VDEBUG_END(log);
	
    	myEntry->action();
    }
}


StateInterface &    
StateInterface::stimulate(
    const Stimulus  &   stimulus
) throw ()
{

    const string    fn("StateInterface::stimuluate");
    VLog    	    log(fn);

    VDEBUG(log) << fn 
    	    	<< "\n\tState: " << myName
    	    	<< ",\t\tStimulus: " << stimulus 
		<< VDEBUG_END(log);
		
    // Lookup the stimulus in the stimulus table. 
    //
    StimulusTable::iterator it = stimulusTable_.find(stimulus);
    
    // If it doesn't exist, do nothing.
    //
    if ( it == stimulusTable_.end() )
    {
    	VDEBUG(log) << fn 
    	    	<< "\n\tState: " << myName
    	    	<< ",\t\tAction: No Action" 
		<< VDEBUG_END(log);

    	return ( *this );
    }
    
    // If the stimulus exists first perform the exit action on this state, 
    // then perform the action associated with the stimulus, and finally
    // perform the entry action on the next state.
    //
    // Note that the exit action, the action associated with this stimulus, 
    // and the entry action of the next state may throw an exception.
    //
    exit();

    Action  &	action = (*it).second.action_;
    
    VDEBUG(log) << fn 
    	    	<< "\n\tState: " << myName
    	    	<< ",\t\tAction: " << action 
		<< VDEBUG_END(log);

    ReturnCode	rc = action.action(stimulus);
    
    StateInterface  	&   nextState = 
    	( rc != SUCCESS && (*it).second.nextStateOnFail_
	    ? *(*it).second.nextStateOnFail_
	    : (*it).second.nextState_ 
    	);

    VDEBUG(log) << fn 
    	    	<< "\n\tState: " << myName
    	    	<< ",\t\tNext State: " << nextState.name() 
		<< VDEBUG_END(log);
    
    nextState.enter();
    
    return ( nextState );
}


void        	    
StateInterface::exit() throw ()
{
    const string    fn("StateInterface::enter");
    VLog    	    log(fn);

    if ( myExit )
    {
    	VDEBUG(log) << fn 
	    	    << "\n\tState: " << myName
	    	    << ",\t\tAction: " << *myExit 
		    << VDEBUG_END(log);
		    
    	myExit->action();
    }
}


ostream &           
StateInterface::writeTo(ostream & out) const
{
    out << name() 
    	<< "\n\tEntry:\t" << ( myEntry ? *myEntry : Action() )
	<< "\n\tExit:\t" << ( myExit  ? *myExit  : Action() )
	<< "\n\tStimuli:\n\t{";
    
    for (   StimulusTable::const_iterator it = stimulusTable_.begin();
    	    it != stimulusTable_.end();
	    it++
	)
    {
    	out << "\n\t\tStimulus       :\t" << (*it).first
	    << "\n\t\tAction         :\t" << (*it).second.action_
	    << "\n\t\tNextState      :\t" << (*it).second.nextState_.name();

    	if ( (*it).second.nextStateOnFail_ )
	{
	    out << "\n\t\tNextStateOnFail:\t"  << (*it).second.nextStateOnFail_->name();
	}

	out << '\n';
    }
    
    return ( out << "\n\t}" );
}


const string &
StateInterface::name() const
{
    return ( myName );
}


StateInterface::StimulusData::StimulusData(
    Action  	    	&   action,
    StateInterface      &   nextState,
    StateInterface      *   nextStateOnFail
)   
    :   action_(action),
    	nextState_(nextState),
	nextStateOnFail_(nextStateOnFail)
{
}


