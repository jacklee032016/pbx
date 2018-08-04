#ifndef UaDevice_Hxx
#define UaDevice_Hxx

/*
 * $Id: UaDevice.hxx,v 1.3 2007/03/03 18:39:28 lizhijie Exp $
 */


#include "Sptr.hxx"
#include "Fifo.h"
#include "SipProxyEvent.hxx"

#include "ResGwDevice.hxx"
//#include "DeviceList.hxx"

namespace Vocal
{

/**
 *  UaDevice
 */
class UaDevice
{
    public:
        ///
        static Sptr < ResGwDevice > instance();

        static void instance( const char* useDevice,
                              Sptr < Fifo < Sptr < SipProxyEvent > > > callProcessingQueue );
        ///
        static Sptr < Fifo < Sptr < SipProxyEvent > > > getDeviceQueue();

	 //below is added by lije 2005-07-07
	 static void LockDevice();
	 static void UnlockDevice();

	 static void setExitFlag(bool bExit);
	 static bool getExitFlag();

	 static void setCanExit(bool bExit);
	 static bool isCanExit();
	 //added ends here	
	
        ///
        ~UaDevice();

    protected:
        ///

    private:
        ///
        UaDevice();

	///	
	static VMutex mutDevice;
	static bool bExitFlag;
	static bool bCanExit;
        ///
        static Sptr < Fifo < Sptr < SipProxyEvent > > > deviceQueue;
        ///
        static Sptr < ResGwDevice > uaDevice;
};
 
}

#endif

