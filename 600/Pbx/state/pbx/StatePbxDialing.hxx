#ifndef __STATE_PBX_DIALING_HXX__
#define __STATE_PBX_DIALING_HXX__
/*
$Id: StatePbxDialing.hxx,v 1.1.1.1 2006/11/29 09:50:50 lizhijie Exp $
*/

#include "State.hxx"


namespace Vocal
{

class StatePbxDialing : public State
{
	public:

		StatePbxDialing();

		virtual ~StatePbxDialing();

		const char* const name() const;
};
 
}
#endif
