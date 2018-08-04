#ifndef STATE_HXX
#define STATE_HXX
/*
 * $Id: State.hxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include <list>
#include "cpLog.h"
#include "Sptr.hxx"
#include "Operator.hxx"
#include "SipProxyEvent.hxx"

namespace Vocal 
{

/* We have circular dependancies, so forward declare.
 */
class State;
class Operator;


/** Return value for process() routine of proxy operators
 *  indicating the processing of the event is complete and no
 *  subsequent operators need to be called. 
 */
extern Sptr<State> PROXY_DONE_WITH_EVENT;


/** Return value for process() routine of proxy operators
 *  indicating the processing of the events is to continue.
 */
extern Sptr<State> PROXY_CONTINUE;


/** Object State
<pre>
<br> Usage of this Class </br>

     It is a operator container. The State::process() calls Operator::process().
     Application in a State can have various operations ( each handled by 
     an operator) that it can perform on the data received.Once operator is 
     done with data it can either flag completion or tell state to call other
     operator in queue to do more processing.Operators are executed in order
     they were added to state.

FooState.hxx:

     class FooState : public Vocal::State
     {
        public:
        FooState();
        virtual ~FooState();
        const char* const name() const;
     };

FooState.cxx:

    using namespace Vocal;

    FooState::FooState()
    {

      Sptr < FooOpAck > opAck           = new FooOpAck;
      addOperator( opAck );

      Sptr < FooOpBye > opBye           = new FooOpBye;
      addOperator( opBye );

      Sptr < FooOpInvite > opInvite     = new FooOpInvite;
      addOperator( opInvite );

      Sptr < FooOpProxy > opProxy       = new FooOpProxy;
      addOperator( opProxy );
   }

   FooState::~FooState()
   {}

   const char* const
   FooState::name() const
   {
     return "FooState";
   }
</pre>
*/
class State
{
    public:

        /** Default constructor
         */
        State();


        /** Virtual destructor
         */
        virtual ~State();


        /** When an event arrives for this state, the typical behavior is
         *  to have each operator associated with this state process the event, 
         *  in the order the operator's were added to this state. If the 
         *  operator returns 0, the routine will check shortCircuitState(). 
         *  If this returns false, which is the default, it will continue 
         *  processing operators, otherwise it will stop processing operators 
         *  and return the next state.
         */
        virtual Sptr < State > process(const Sptr < SipProxyEvent > event);


        /** The extender of this class must provide the name of this
         *  class via this abstract method.
         */
        virtual const char* const name() const
        {
            return ( "State" );
        }


        /** Logging helper method
         */
        virtual void doCpLog() const
        {
            cpLog( LOG_DEBUG, "State: %s", name() );
        }


    protected:


        /** Add an operator to this state.
         */
        void addOperator(const Sptr < Operator > op);


        /** An operator whose process method will be called when the 
         *  state is entered. The returned state from the operator's process
         *  is ignored.
         */
        void addEntryOperator(const Sptr < Operator > op);


        /** An operator whose process method will be called when the 
         *  state is exited. The returned state from the operator's process
         *  is ignored.
         */
        void addExitOperator(const Sptr < Operator > op);


        /** All operators, including entry and exit, and forgotten.
         */
        void clearAll( );


        /** Called upon entry into the state. All associated entry operators'
         *  process methods are called. They will be called in the order they
         *  have been added to the state. There is not short circuiting
         *  operators.
         */
        virtual void processEntry(const Sptr < SipProxyEvent > event);


        /** Called upon exit from the state. All associated exit operators'
         *  process methods are called. They will be called in the order they
         *  have been added to the state. There is not short circuiting
         *  operators.
         */
        virtual void processExit(const Sptr < SipProxyEvent > event);


        /** List of operators.
         */
        typedef list < Sptr < Operator > > OperatorContainer;


        /** Iterator for the operator list. Yes, it's poorly named.
         */
        typedef OperatorContainer::iterator OperatorIter;


        /** Contains operators to execute on state entry
         */
        OperatorContainer       myEntryOperators;    


        /** Contains operators to handle events
         */
        OperatorContainer       myOperators;


        /** Contains operators to execute on state exit
         */
        OperatorContainer       myExitOperators;   


    private:


        /** Add an operator (op) to the given container, logging the
         *  given logMessage.
         */
        void helperAdd( OperatorContainer       &   container, 
	    	        const Sptr < Operator > &   op, 
			const char* const           logMessage);


        /** Suppress copying
         */
        State(const State &);


        /** Suppress copying
         */
        const State & operator=(const State &);
};
 
}

#endif // STATE_HXX
