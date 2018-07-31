#ifndef __PBX_DEVICE_EVENT_HXX__
#define __PBX_DEVICE_EVENT_HXX__
/*
 * $Id: PbxDeviceEvent.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "PbxEvent.hxx"

namespace Assist
{

//class CtrlDevice;

/* PbxEvent send to Device Write Thread */
class PbxDeviceEvent : public PbxEvent
{
	public:
		PbxDeviceEvent();

		PbxDeviceEvent( const Sptr < Fifo < Sptr <PbxEvent> > > outputFifo );

		virtual ~PbxDeviceEvent();

		virtual const char* const name() const;

	protected:

	private:
//		Sptr <CtrlDevice> 			myCtrlDevice;
		
};
 
}

#endif

