/*
* $Id: EpOpBuilder.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#ifndef	__EP_OP_BUILDER_HXX__
#define	__EP_OP_BUILDER_HXX__

#include "EpOperator.hxx"

#include "EndPoint.hxx"
#include "PbxState.hxx"
#include "PbxTimerEvent.hxx"
#include "DigitEvent.hxx"
#include "CallEvent.hxx"
#include "CallMsgBuilder.hxx"


namespace Assist
{

#define EP_OP_DECLARE(type, name) \
class type : public EpOperator\
{ \
public: \
	type() : EpOperator(name) {} \
	~type(){} \
	virtual const Sptr <PbxState>  process(const Sptr <PbxEvent> event); \
	private : int myDelay; \
}


	EP_OP_DECLARE(EpOpAddDigit, "EpOpAddDigit");
	EP_OP_DECLARE(EpOpCancelTimer, "EpOpCancelTimer");

	#define EP_OP_DECLARE_TIMER(type, name) \
class type : public EpOperator\
{ \
public: \
	type(int delay) : EpOperator(name) {myDelay = delay;} \
	~type(){} \
	virtual const Sptr <PbxState>  process(const Sptr <PbxEvent> event); \
	private : int myDelay; \
}


	EP_OP_DECLARE_TIMER(EpOpStartTimer, "EpOpStartTimer");


	#define EP_OP_DECLARE_RELEASE_COMPLETE(type, name) \
class type : public EpOperator\
{ \
public: \
	type(int nextState) : EpOperator(name) {myNextState = nextState;} \
	~type(){} \
	virtual const Sptr <PbxState>  process(const Sptr <PbxEvent> event); \
	private : int myNextState; \
}

	EP_OP_DECLARE_RELEASE_COMPLETE(EpOpCallReleaseComplete, "EpOpCallReleaseComplete");

}

#endif

