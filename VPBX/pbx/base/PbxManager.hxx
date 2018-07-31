#ifndef __PBX_MANAGER_HXX__
#define __PBX_MANAGER_HXX__
/*
* $Id: PbxManager.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include <Sptr.hxx>
#include "vpbx_globals.h"

#include "PbxEvent.hxx"
#include "PbxManagerThread.hxx"
#include "PbxConfiguration.hxx"

namespace Assist
{

class PbxTimerEvent;
class PbxAgent;
class PbxStateMachine;

/* Base class of all manager object in VPBX, including DeviceMgr, GatewayMgr, Call Scheduler */
class PbxManager 
{
	public:
		PbxManager( PbxAgent *agent, const char *_name);
		virtual ~PbxManager();

		const char *getName() const;

		Sptr < Fifo < Sptr <PbxEvent> > > getQueue();
		
		Sptr <PbxTimerEvent>  createTimerEvent();

		
		virtual void  mainLoop( void* parms );
		void  processEvent( Sptr <PbxEvent> event );

		void execute();
		void join();
		void shutdown();

		
		Sptr <PbxStateMachine>  getPbxStateMachine();

		void setPbxStateMachine(PbxStateMachine *stateMachine);

		/* report event to callscheduler, mutex access by different thread */
		void reportEvent( Sptr <PbxEvent> event);
		
	protected:
		/* child class must implement this function, it can not be blocked forever in order to process PbxEvent from other thread */
		virtual void myMainLoop(void *params) = 0;
		
		virtual void myProcessEvent( Sptr <PbxEvent> event ) = 0;

		/* some initial action execute before thread main loop, return 0 : success, other:  fail and exit */
		virtual int startup() =0;

		/* added mutex here */
        	VMutex 							myMutex;
		char								myName[VPBX_NAME_LENGTH];
	private:
		

		Sptr < Fifo < Sptr <PbxEvent> > > 	myQueue;
		
		Sptr <PbxManagerThread>			myThread;

		Sptr <PbxAgent>					myAgent;


		/*
		* global unique state machine instance for this device type, ISDN, PSDN, Call,Voip and Virtual Console 
		* StateMachine for StateObject, not PbxManager itself
		*/
		Sptr <PbxStateMachine>			myPbxStateMachine;

};
 
}

#endif

