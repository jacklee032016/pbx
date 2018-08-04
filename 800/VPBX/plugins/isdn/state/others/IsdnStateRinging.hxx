#ifndef __ISDN_STATE_RINGING_HXX__
#define __ISDN_STATE_RINGING_HXX__

/*
 * $Id: IsdnStateRinging.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include <cassert>
#include "IsdnState.hxx"


namespace Assist
{

class IsdnStateRinging : public IsdnState
{
	public:
		IsdnStateRinging();
		virtual ~IsdnStateRinging();

		const char* const name() const;
};
 
}

#endif
