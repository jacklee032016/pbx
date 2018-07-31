#ifndef __CALL_OPERATOR_HXX__
#define __CALL_OPERATOR_HXX__

/*
 * $Id: CallOperator.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "cpLog.h"
#include <PbxOperator.hxx>
#include "CallInfo.hxx"
#include "CallContainer.hxx"

namespace Assist
{

/* common super class for all Call Operations */
class CallOperator : public PbxOperator
{
	public:
		CallOperator(const string& name);
		virtual ~CallOperator();

	protected:
		Sptr <CallInfo> getCallInfo(Sptr <PbxEvent> event);

		void forwardCallEvent(Sptr <EndPoint> fwdEp, Sptr <PbxEvent> event);

		void sendCallSignalToMgr(Sptr <EndPoint> destEp, call_method_t  method);
		
	private:
		CallOperator(const CallOperator& rhs);
		CallOperator& operator=(const CallOperator& rhs);
};
 
}

#endif

