#ifndef __PBX_STATE_HXX__
#define __PBX_STATE_HXX__
/*
 * $Id: PbxState.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */
#include <list>

#include "PbxOperator.hxx"

namespace Assist 
{

#define		PBX_STATE_DONE_ID					0x1000
#define		PBX_STATE_INVALIDATE_ID			-1

class PbxState;

extern Sptr <PbxState> PBX_STATE_DONE_WITH_EVENT;

extern Sptr <PbxState> PBX_STATE_CONTINUE;


/* Common super class for all State class in VPBX
 * EpState and CallState are its direct child class
*/
class PbxState
{
	public:
		PbxState(const string& name, int state = PBX_STATE_INVALIDATE_ID);
		virtual ~PbxState();

		/* handle the PbxEvent */
		virtual Sptr <PbxState> process(const Sptr <PbxEvent> event);

		
		virtual void doCpLog() const;

		/*
		* In order to declare PBX_STATE_DONE_WITH_EVENT, following 2 functions can not 
		* be defined as abstract 
		*/
		virtual int getStateType()
		{
			return myState;
		};
		
		virtual const char* const name() const
		{
			return myName.c_str();
		};


		/* called in process() function, in EpState return EndPoint data member, in CallState return PbxStateObject data member */
		virtual Sptr <PbxStateObject>  getMyStateObject(const Sptr <PbxEvent> event) ;
		
		virtual void  setMyStateObject(const Sptr <PbxEvent> event, Sptr <PbxStateObject> obj) ;
		
	protected:
		void addOperator(const Sptr <PbxOperator> op);		

		/** An operator whose process method will be called when the 
		*  state is entered. The returned state from the operator's process is ignored. */
		void addEntryOperator(const Sptr <PbxOperator> op);
		void addExitOperator(const Sptr <PbxOperator> op);

		void clearAll( );

		/** Called upon entry into the state. All associated entry operators'
		*  process methods are called. They will be called in the order they
		*  have been added to the state. There is not short circuiting
		*  operators.         */
		virtual void processEntry(const Sptr <PbxEvent> event);
		virtual void processExit(const Sptr <PbxEvent> event);

		typedef list < Sptr <PbxOperator> > OperatorContainer;
		typedef OperatorContainer::iterator OperatorIter;

		OperatorContainer       myEntryOperators;
		OperatorContainer       myOperators;
		OperatorContainer       myExitOperators;   

//		virtual void initState(){};
		
	private:
		string 	myName;
		int		myState;
		
		void helperAdd( OperatorContainer &   container, const Sptr <PbxOperator> &   op, const char* const  logMessage);

		PbxState(const PbxState &);
		const PbxState & operator=(const PbxState &);
};
 
}

#endif

