#ifndef __PSTN_DEVICE_MGR_HXX__
#define __PSTN_DEVICE_MGR_HXX__
/*
* $Id: PstnDeviceMgr.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "PbxHwEvent.hxx"
#include "PbxDeviceEvent.hxx"
#include "PstnEvent.hxx"

#include "assist_lib.h"
#include "pstn.hxx"

#include "DeviceMgr.hxx"

namespace Assist
{

class PstnDeviceMgr : public DeviceMgr
{
	public:
		PstnDeviceMgr(PbxAgent* agent );
		virtual ~PstnDeviceMgr();
		
		void downProcessMain( void *params );

		virtual void mainLoop (void* params);  
		
		virtual Sptr <EndPoint>  lookupEndPointByNumber(const char *externalPhoneNumber);
		void myProcessHwEvent( Sptr <PbxHwEvent > event );
	
	protected:
		void myMainLoop(void *params) ;

		virtual void activate()
		{
		};
		
		virtual int init();

		int startup()
		{
			return 0;
		};
		
	private:
		void destory(void);

		int addToFdSet (fd_set* fd);
		void reportHardwareDtmfEvent(as_device_t *dev);

		int 						myDebugOption;
		
		as_span_t 				*span;
	
};
 
}

#endif

