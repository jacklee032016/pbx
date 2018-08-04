#ifndef  __CTRL_DEVICE_HXX__
#define __CTRL_DEVICE_HXX__
/*
* $Id: CtrlDevice.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "Sptr.hxx"
#include <VMutex.h>
#include "VException.hxx"

#include "vpbx_globals.h"

#include "EndPoint.hxx"
#include "AudioChannel.hxx"

namespace Assist
{

class DeviceMgr;
class PbxDeviceEvent;
class PbxEvent;

/* Abstract base class for Control Device, such as an D Channel */
class CtrlDevice 
{
	public:
		CtrlDevice(DeviceMgr *_parent , int index, char *_name);
		virtual ~CtrlDevice();

		/* send HW command to device */
		virtual void sendHwCommand( Sptr <PbxEvent> hwCommand) = 0;

		/* return : 1, msg has been handled, 0, not handled */
		virtual int hardwareMain( void *params) = 0;
		
		const char *getName() const
		{
			return myName;
		};

#if WITH_DEBUG
		virtual const char *debugInfo();
#endif

		void processAudio();

		Sptr <AudioChannel> getFreeAudio();
		virtual Sptr <AudioChannel> getNextAudio( Sptr <AudioChannel> _audio) ;

		virtual Sptr <EndPoint> getNextEP( Sptr <EndPoint> _ep);
		virtual Sptr <EndPoint> lookupFreeEP() =0;

		virtual Sptr <EndPoint> lookupEPByNumber(const char *_number);
		virtual void setExtPhoneNumber(const char *_extPhone);


		void setIndex(int _index)
		{
			myIndex = _index;
		};

		int getIndex()
		{
			return myIndex;
		};

		Sptr <DeviceMgr> 	getParent();

		virtual int init() = 0 ;

		/* after register in Gateway, this function is called to startup, 
		* return 0 ,successfully ,lizhijie,2005.12.28  */
		virtual int startup();
		
	protected:

		Sptr <AudioChannel>		myAudioChan;
		Sptr <EndPoint> 			myEndPoint;

		VMutex 					myAudioMutex;
		VMutex 					myActionMutex;
		
		int 						myAudioNum; 				/* number of Audio channels */
		int 						myUseRef; 					/* counts the number of port that uses this port */
		
		/* default : save the EXT phone number of this device, for line device, it is useless */
		char						myPhoneNumber[VPBX_NAME_LENGTH];
	private:
		Sptr <DeviceMgr> 			myParent;


		int 						myIndex;				/* assigned in GatewayMgr, begin from 0 */
		char 					myName[VPBX_NAME_LENGTH];

};

}

#endif

