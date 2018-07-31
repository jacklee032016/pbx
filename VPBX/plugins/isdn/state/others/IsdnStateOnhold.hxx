#ifndef __ISDN_STATE_ONHOLD_HXX__
#define __ISDN_STATE_ONHOLD_HXX__
/*
 * $Id: IsdnStateOnhold.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "IsdnState.hxx"

namespace Assist
{

class IsdnStateOnhold : public IsdnState
{
	public:
		IsdnStateOnhold();
		virtual ~IsdnStateOnhold();

		const char* const name() const;
};
 
}

#endif
