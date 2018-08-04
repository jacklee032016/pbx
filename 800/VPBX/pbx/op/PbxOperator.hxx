#ifndef __PBX_OPERATOR_HXX__
#define __PBX_OPERATOR_HXX__

/*
 * $Id: PbxOperator.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */
#include "Sptr.hxx"
#include "cpLog.h"
#include "CommonMsg.hxx"

namespace Assist
{

class PbxEvent;
class CallEvent;
class PbxState;

/* Common super class for all operation, such as EpOperator and CallOperator  */
class PbxOperator 
{
	public:
		PbxOperator(const string& name);
		virtual ~PbxOperator();

		virtual const char* const name() const ;

		/** Process the incoming event. */
 		virtual const Sptr <PbxState> process(const Sptr <PbxEvent> event) = 0;

		virtual void doCpLog(const Sptr <PbxEvent> event ) const;

		//virtual Sptr < Fifo < Sptr <PbxEvent> > > getTimerFifo();

	protected:
#if 0	
		virtual bool setTimer( const Sptr <PbxEvent> event, unsigned int timeout );

		virtual bool cancelTimer( const Sptr <PbxEvent> event );
		virtual bool isTimeout( const Sptr <PbxEvent> evnet );
#endif		
		/* when event is a CallEvent, then CommonMsg in it is return; otherwise, return NULL*/
		virtual call_method_t checkCallMsg( const Sptr <PbxEvent> event);
		virtual Sptr <CommonMsg> isCallMsg( const Sptr <PbxEvent> event);
		virtual Sptr <CallEvent> isCallEvent( const Sptr <PbxEvent> event);

		virtual  void sendGatewayEvent(Sptr <PbxEvent> event);
		
	protected:

	private:
		string 					myName;
		
		PbxOperator(const PbxOperator& rhs);
		PbxOperator& operator=(const PbxOperator& rhs);
};
 
}

#endif

