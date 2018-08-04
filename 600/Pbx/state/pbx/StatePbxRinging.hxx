#ifndef __STATE_PBX_RINGING_HXX__
#define __STATE_PBX_RINGING_HXX__
/*
$Id: StatePbxRinging.hxx,v 1.1.1.1 2006/11/29 09:50:50 lizhijie Exp $
*/

#include "State.hxx"


namespace Vocal
{

class StatePbxRinging : public State
{
	public:

		StatePbxRinging();

		virtual ~StatePbxRinging();

		const char* const name() const;
};
 
}
#endif
