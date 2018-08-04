#ifndef __EP_STATE_OVERLAP_OUT_HXX__
#define __EP_STATE_OVERLAP_OUT_HXX__
/*
 * $Id: EpStateOverLapOut.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "IsdnState.hxx"

namespace Assist
{
/* need more information in SETUP msg */
class EpStateOverlapOut : public EpStateSrc
{
	public:
		EpStateOverlapOut();
		~EpStateOverlapOut();
		const char* const name() const;
		
		int getStateType()
		{
			return ISDN_EP_STATE_OUT_OVERLAP;
		}		
};
 
}
#endif

