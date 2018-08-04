#ifndef __CALL_STATE_HXX__
#define __CALL_STATE_HXX__
/*
 * $Id: CallState.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "CallOperator.hxx"
#include "CallInfo.hxx"
#include "PbxState.hxx"

namespace Assist 
{

/* We have circular dependancies, so forward declare.
 */
class CallState;

/* Common super class for Virtual Call state In VPBX environments */
class CallState : public PbxState
{
	public:
		CallState(const string& name, int state);
		virtual ~CallState();

		virtual Sptr <PbxStateObject>  getMyStateObject(const Sptr <PbxEvent> event) ;
		virtual void  setMyStateObject(const Sptr <PbxEvent> event, Sptr <PbxStateObject> obj) ;

		virtual void initState() = 0;
	protected:
		
	private:
		CallState(const CallState &);
		const CallState & operator=(const CallState &);
};

#define CALL_STATE_DECLARE(type, name, state)  \
class type : public CallState\
{ \
public: \
	type() : CallState(name, state) { initState(); }; \
	~type(){}; \
	virtual void  initState(); \
}

}

#endif

