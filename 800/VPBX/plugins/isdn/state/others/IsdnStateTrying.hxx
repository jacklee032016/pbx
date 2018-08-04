#ifndef __ISDN_STATE_TRYING_HXX__
#define __ISDN_STATE_TRYING_HXX__
/*
 * $Id: IsdnStateTrying.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include <cassert>
#include "IsdnState.hxx"

namespace Assist
{

class IsdnStateTrying : public IsdnState
{
	public:
		IsdnStateTrying();
		virtual ~IsdnStateTrying();

		const char* const name() const;
};
 
}

#endif
