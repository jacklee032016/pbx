/*
 * $Id: EpOperator.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#ifndef __EP_OPERATOR_HXX__
#define __EP_OPERATOR_HXX__

#include "PbxOperator.hxx"

namespace Assist
{

class PbxEvent;
class PbxTimerEvent;
class DigitEvent;
class PbxState;

/* Common Super class for All EndPoint Operation */
class EpOperator : public PbxOperator
{
	public:
		EpOperator(const string& name);
		virtual ~EpOperator();

	protected:
		virtual bool setTimer( const Sptr <PbxEvent> event, unsigned int timeout );

		virtual bool cancelTimer( const Sptr <PbxEvent> event );
#if 0
		virtual bool isTimeout( const Sptr <PbxEvent> event );
#endif
		virtual Sptr <PbxTimerEvent> isTimeout( const Sptr <PbxEvent> event );
		virtual Sptr <DigitEvent> isDigit(const Sptr <PbxEvent> event);

		void sendCallSignal(Sptr <PbxEvent> event, call_method_t cmd);
		virtual  void sendSchedulerEvent(Sptr <PbxEvent> event);

		virtual  void sendDeviceCommand(Sptr <PbxEvent> event);

		Sptr <PbxState> lookupEPState(Sptr <PbxEvent> event,  int stateType);
		
	private:
		EpOperator(const EpOperator& rhs);
		EpOperator& operator=(const EpOperator& rhs);
};

}

#endif

