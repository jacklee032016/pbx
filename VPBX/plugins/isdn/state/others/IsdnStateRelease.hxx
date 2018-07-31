#ifndef __ISDN_STATE_RELEASE_HXX__
#define __ISDN_STATE_RELEASE_HXX__
/*
 * $Id: IsdnStateRelease.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include <cassert>
#include "IsdnState.hxx"


namespace Assist
{

/* call released */
class IsdnStateRelease : public IsdnState
{
	public:
		IsdnStateRelease();
		virtual ~IsdnStateRelease();

		const char* const name() const;
};
 
}
#endif

