#ifndef __DEVICE_MGR_HXX__
#define __DEVICE_MGR_HXX__
/*
* $Id: DeviceMgr.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "Sptr.hxx"
#include "Fifo.h"
#include <vector>
using std::vector;

#include "vpbx_globals.h"

#include "PbxEvent.hxx"

#include "PbxManager.hxx"

#include "CtrlDevice.hxx"

namespace Assist
{

class GatewayMgr;
class PbxHwEvent;

//typedef list < Sptr <CallId> > 			CallWaitingIdList;

typedef vector < Sptr <CtrlDevice> > 		DeviceList;

/* Base class for different Device Manager : 
* There is a standalone device monitor thread for every DeviceMgr 
*/
class DeviceMgr : public PbxManager
{
	public:
		DeviceMgr(PbxAgent *agent );
		virtual ~DeviceMgr();

		/************  Main Loop 1 : monitor device msg : defined in PbxManager **********/
		
		/************  Main Loop 2 : send msg to hardware ********/
//		virtual void downProcessMain( void *params) = 0;

		/************  Main Loop 3 : Audio process ***************/
		virtual void processAudio(void );
		
		void setUsePolicy( bool flag )
		{
			myUsePolicy = flag; 
		};

		bool getUsePolicy()
		{
			return myUsePolicy;
		};


		void setIndex(int _index)
		{
			myIndex = _index;
		};

		int getIndex()
		{
			return myIndex;
		};

		int getDeviceCount()
		{
			return myDeviceCount;
		};

		/* only externel line phone number is lookup by DeviceMgr. Extension phone number is checked by GatewayMgr 
		*   return a free EndPoint, else return null 
		*/
		virtual Sptr <EndPoint>  lookupEndPointByNumber(const char *externalPhoneNumber) = 0;
		virtual Sptr <EndPoint>  findEpbyNumber(const string& num);
		virtual Sptr <EndPoint>  findFreeEP();

		Sptr <CtrlDevice> getNextDevice(const Sptr <CtrlDevice> dev);
		Sptr <CtrlDevice> getNextDevice(int index);

		Sptr <GatewayMgr>	getGatewayMgr();

		const char *getAccessCode();
		void setAccessCode(const char *_accessCode);

		virtual int startup();

		bool	checkSecondDial()
		{
			return mySupportSecondDial;
		};

		void setName(const char *name);
		
		DialMethodsType getDialMethodType();
#if WITH_DEBUG
		virtual const char *debugInfo();
#endif

	protected:
		void addDevice( CtrlDevice *newCtrlDev);
		Sptr<CtrlDevice> getDevice(int deviceId ) ;
		void removeDevice( const Sptr <CtrlDevice> ctrlDevice);

		virtual void activate() =0;
		virtual int init() = 0;


		/********  following 2 functions must be implemented in child class in order to execute hardwareMain loop  ********/
		void myProcessEvent( Sptr <PbxEvent> event ) ;
		/* Process a event send to queue of this thread, child class must implement this function */
		virtual void myProcessHwEvent( Sptr <PbxHwEvent> hwEvent ) = 0;

		void setDeviceCount(int _count)
		{
			myDeviceCount = _count;
		};

		Sptr <GatewayMgr>				myGatewayMgr;
		
		char								accessCode[MAX_ACCESS_CODE_LENGTH];
		
	protected:
#if 0	
		/// Call Id on call waiting list
		Sptr<CallWaitingIdList> callWaitingIdList;

#endif
		virtual void destory(void) = 0;

		bool 							myUsePolicy;


		int 								myIndex;		/* assigned in GatewayMgr, begin from 0 */
		
		/* input queue for every DeviceMonitor thread  */
		int 								myDeviceCount;
		DeviceList						myDeviceList;

		/* for ISDN and PSTN, first dial access code, then after hearing DT from carrier switch and dial again 
		* it is used in DialingValidate and EndPoint lookup 
		*/
		bool								mySupportSecondDial;
		DialMethodsType					myDialMethodType;
};
 
}

#endif

