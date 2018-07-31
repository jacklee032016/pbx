#ifndef  __PBX_STATEMACHINE_HXX__
#define __PBX_STATEMACHINE_HXX__
/*
 * $Id: PbxStateMachine.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "Sptr.hxx"
#include "cpLog.h"
#include "PbxStateObject.hxx"
#include "PbxState.hxx"

namespace Assist
{

class PbxEvent;
class PbxManager;

/* EndPoint State Machine, management EndPoint state */
class PbxStateMachine 
{
	public:
		PbxStateMachine(PbxManager  *_parent);
		virtual ~PbxStateMachine();

//		virtual int getStateType() = 0;
			
		virtual void process( const Sptr <PbxEvent> event );
		
		Sptr <PbxState> findState( const string stateName );

		Sptr <PbxState> findState( int state );
		
		/** Add a state machine to thus state machine */
		void addState(const Sptr <PbxState> state);

		Sptr <PbxManager> getParent() ;


	protected:
		typedef list < Sptr <PbxState> > 	StateContainer;

		typedef StateContainer::iterator 	StateIter;

		/** Contains state machines to handle events.*/
		StateContainer 					myStates;

		virtual Sptr <PbxStateObject> getMyStateObject(Sptr <PbxEvent> evnet);
	private:
		Sptr <PbxManager>				myParent;

		PbxStateMachine(const PbxStateMachine &);

//		const PbxStateMachine & operator=(const PbxStateMachine &);

};
 
}

#endif

