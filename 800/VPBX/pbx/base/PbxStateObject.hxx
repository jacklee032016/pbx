#ifndef __PBX_STATE_OBJECT_HXX__
#define __PBX_STATE_OBJECT_HXX__
/*
* $Id: PbxStateObject.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "ManagedObject.hxx"
//#include "PbxState.hxx"

namespace Assist 
{

class PbxStateMachine;
class PbxState;
class PbxEvent;
class PbxManager;
class PbxTimerEvent;


/* Object with state in our vpbx environments, such as EndPoint and CallInfo  */
class PbxStateObject: public ManagedObject
{
	public:
		PbxStateObject( );
		PbxStateObject(Sptr <PbxManager> _parent );

		virtual ~PbxStateObject();
		
		/*
		* transimission of state is implemented in PbxStateMachine, so no PbxState is return
		*/
		virtual void process( Sptr <PbxEvent> msg);

		/* set new state in this endPoint: call exit_operation in the prior state 
		and enter_operation in the next state, called in deviceThread context */
		void setState(Sptr <PbxState> _state);

		/* following functions are called in Call Scheduler thread context */
		Sptr <PbxState> getState();

		Sptr <PbxState> findState(int stateType);

		virtual const char *getName() = 0;
		


		void setTimer(const Sptr <PbxTimerEvent> timerEvent);
		Sptr <PbxTimerEvent> getTimer() const;
		void removeTimer();

		
		/** Call mutex lock on PbxStateObject */
		void lock();

		/** Call mutex unlock on PbxStateObject */
		void unlock(); 
		
		Sptr <PbxStateMachine> getStateMachine();
	protected:
		

	private:
		Sptr <PbxStateMachine>    	myStateMachine;
		
		Sptr <PbxState> 			myState;

		VMutex 					myMutex;

//		Sptr <EndPoint>			myEndPoint;
//		Sptr <ManagedObject>		myEndPoint;

		Sptr <PbxTimerEvent> 		myTimer;		

};

}

#endif

