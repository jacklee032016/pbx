#ifndef __CONSOLE_DEVICE_MGR_HXX__
#define __CONSOLE_DEVICE_MGR_HXX__
/*
* $Id: ConsoleDeviceMgr.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "PbxHwEvent.hxx"
#include "PbxDeviceEvent.hxx"

#include "DeviceMgr.hxx"

namespace Assist
{

class ConsoleDeviceMgr : public DeviceMgr
{
	public:
		ConsoleDeviceMgr(PbxAgent *agent );
		virtual ~ConsoleDeviceMgr();
		
		Sptr <CtrlDevice> getDeviceByAddress( unsigned int address);

		/* called by callScheduler to routing a call */		
		Sptr <EndPoint> lookupEndPointByNumber(const char *extPhoneNumber);
		virtual Sptr <EndPoint>  findFreeEP();

		int getMyFd()
		{
			return myFd;
		}


	protected:
		void myMainLoop(void *params) ;

		virtual void activate()
		{
		};
		
		virtual int init();


		void myProcessEvent( Sptr <PbxEvent> event ) ;

		/* this function is not used  */
		void myProcessHwEvent( Sptr <PbxHwEvent > event );

		int startup()
		{
			return 0;
		};

	private:
		void destory(void);

		int 						myFd;	/* for D channel fd of all ports */
		bool 					originator;

};
 
}

#endif

