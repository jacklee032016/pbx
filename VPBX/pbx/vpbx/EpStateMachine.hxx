#ifndef  __EP_STATEMACHINE_HXX__
#define __EP_STATEMACHINE_HXX__
/*
 * $Id: EpStateMachine.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "PbxStateMachine.hxx"

namespace Assist
{

/* EndPoint State Machine, management EndPoint state, base class for state machine of ISDN,PSTN and virtual console */
class EpStateMachine : public PbxStateMachine
{
	public:
		EpStateMachine(PbxManager  *_parent);
		~EpStateMachine();

	protected:
/*		
		virtual int getStateType() 
		{
			return 0;
		};
			
		const char* getName() const;
*/
		virtual Sptr <PbxStateObject> getMyStateObject(Sptr <PbxEvent> evnet);

	private:
		EpStateMachine(const EpStateMachine &);
/*
		const EpStateMachine & operator=(const EpStateMachine &);

		static PbxStateMachine *stateMachineInstance;
*/		
};
 
}

#endif

