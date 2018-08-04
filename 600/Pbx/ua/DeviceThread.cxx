/*
 * $Id: DeviceThread.cxx,v 1.7 2007/03/30 09:13:18 lizhijie Exp $
 */

#include "global.h"
#include <cassert>
#include "aslib.h"

#include "DeviceThread.hxx"
#include "UaDevice.hxx"
#include "UaConfiguration.hxx"
#include "UaBuilder.hxx"
#include "RegistrationManager.hxx"

using namespace Vocal;

///
DeviceThread::DeviceThread( const Sptr < ResGwDevice > hwDevice )
{
	device = hwDevice;
}

///
DeviceThread::~DeviceThread()
{}

void DeviceThread::thread()
{
	int time;
	bool bExit = false;
	
	cpLogSetLabelThread (VThread::selfId(), "Assist HW Thread");

  	cpDebug(LOG_DEBUG, "PID of Assist HWThread is %d" ,getpid() );

#if __ARM_IXP__
  	//added by lijie to enable watch dog function 2005-07-05
	time = UaConfiguration::instance()->getWatchDogTime();
	if(time<10)
		time = 10;
	as_watchdog_enable(time , 0);
	//added ends here
#endif

	while ( true )
	{
		UaDevice::LockDevice();
		if(UaDevice::isCanExit()&&UaDevice::getExitFlag())
		{
			bExit = true;
		}
		UaDevice::UnlockDevice();

		if(bExit)
		{
#if __ARM_IXP__
			as_watchdog_disable();
#endif
			if ( UaConfiguration::instance()->getRegisterOn() )
		    	{
				cpLog(LOG_EMERG, "SIP UA is shutdown by User Signal!");
		    		/* added following for unregistered when SIP UA is shutdown, lzj, 2007.03.28  */
		    		Sptr <RegistrationManager> regMgr = UaBuilder::getRegManager();
				if(regMgr!=NULL)
				{
					regMgr->unRegister();
				}
				/* delay for sime time to wait workerThread reply authen unregister */
//				sleep(10);
				
				
				//exit(1);
				
				//shutdown();
				/* end of added */
			}
			else
			{
				::exit(-1);
			}
		}
#if __ARM_IXP__
		 as_watchdog_update();
#endif

		device->hardwareMain( 0 );

		if ( isShutdown() == true)
		{
			cpLog(LOG_WARNING, "is shutdown");
			return;
		}	
	}
}

