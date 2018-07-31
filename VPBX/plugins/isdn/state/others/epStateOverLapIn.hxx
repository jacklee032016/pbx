#ifndef __EP_STATE_OVERLAP_IN_HXX__
#define __EP_STATE_OVERLAP_IN_HXX__
/*
 * $Id: epStateOverLapIn.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "IsdnState.hxx"

namespace Assist
{

class EpStateOverlapIn : public EpStateDest
{
	public:
		EpStateOverlapIn();
		~EpStateOverlapIn();
		const char* const name() const;
		
		int getStateType()
		{
			return ISDN_EP_STATE_IN_OVERLAP;
		}		
};
 
}
#endif

