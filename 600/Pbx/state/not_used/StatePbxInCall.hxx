#ifndef __STATE_PBX_IN_CALL_HXX__
#define __STATE_PBX_IN_CALL_HXX__

/*
$Id: StatePbxInCall.hxx,v 1.1.1.1 2006/11/29 09:50:50 lizhijie Exp $
 */
#include "State.hxx"

namespace Vocal
{

class StatePbxInCall : public State
{
	public:
		StatePbxInCall();

		virtual ~StatePbxInCall();

		const char* const name() const;
};
 
}

#endif
