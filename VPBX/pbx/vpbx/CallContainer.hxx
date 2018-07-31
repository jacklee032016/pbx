#ifndef __PBXCALL_CONTAINER_HXX__
#define __PBXCALL_CONTAINER_HXX__
/*
 * $Id: CallContainer.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */
#include "Sptr.hxx"
#include "cpLog.h"

namespace Assist
{

class CallId;
class CallInfo;

/*
* Only one CallContainer instance exist in our system, which is contained in CallScheduler
*/
class CallContainer
{
	public:
		CallContainer();
		virtual ~CallContainer();

		virtual Sptr <CallInfo> getCall(const Sptr <CallId> newOrExistingCall);

		virtual Sptr <CallInfo> findCall(const Sptr <CallId> newOrExistingCall);

		virtual Sptr <CallInfo> addCall(const Sptr <CallId> _callid, const Sptr <CallInfo> _callInfo);

		virtual void removeCall(const Sptr <CallId> _callId);

    protected:
		/** typedef map of CallId pointers, using a SipCallLeg key */
#if 1		
		typedef map < Sptr<CallId>, Sptr <CallInfo> > 	Table;
#else
		typedef map < CallId, Sptr < CallInfo > > Table;
#endif

		/** Table iterator 		*/
		typedef Table::iterator 						TableIter;

		/** CallContainer map object */
		Table 										myCallInfos;
		
		/** used to lock access to the CallContainer data.*/
		mutable VMutex 								myMutex;

	private:
		/** Suppress copying*/
		CallContainer(const CallContainer &);
		const CallContainer & operator=(const CallContainer &);
};
 
}

#endif

