#ifndef __VOIP_DEVICE_MGR_HXX__
#define __VOIP_DEVICE_MGR_HXX__
/*
* $Id: VoipDeviceMgr.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "PbxHwEvent.hxx"
#include "PbxDeviceEvent.hxx"

#include "DeviceMgr.hxx"
#include "SipCallId.hxx"

namespace Assist
{

class VoipEndPoint;
class SipTransceiver;
class UaBuilder;

class VoipDeviceMgr  : public DeviceMgr
{
	public:
		VoipDeviceMgr(PbxAgent* agent, bool callLegHistory=true );
		virtual ~VoipDeviceMgr();

		virtual Sptr <EndPoint>  lookupEndPointByNumber(const char *extPhoneNumber);
//		Sptr <VoipEndPoint> findEpByServiceObj(const string& obj);
		void myProcessHwEvent( Sptr <PbxHwEvent > event )
		{
		}
	
		virtual Sptr <VoipEndPoint>  getEndPointByPhoneNumber(const char *phonenumber, bool isInvite);

		virtual Sptr <VoipEndPoint>  getEndPointBySipCallId(SipCallId &sipCallId);

		virtual Sptr <EndPoint>  findFreeEP();
	
	protected:
		virtual void mainLoop (void* params);  
		void myMainLoop(void *params) ;

		virtual void activate()
		{
		};
		
		virtual int init();

		int startup()
		{
			return 0;
		};
		
		void destory(void)
		{
		}
		
	private:
//		Sptr <UserAgent> 					ua;

		Sptr <SipTransceiver>				mySipStack;
		Sptr <UaBuilder> 					myUaBuilder;
		

		bool 							myCallLegHistory;
};
 
}

#endif

