#ifndef __ASSIST_INVALID_DEVICE_H__
#define __ASSIST_INVALID_DEVICE_H__

/*
 * $Id: AssistInvalidDevice.hxx,v 1.1.1.1 2006/11/29 09:50:50 lizhijie Exp $
*/
#include <VMutex.h>
#include "VException.hxx"

#include <Sptr.hxx>
#include <SipProxyEvent.hxx>
#include <SipCallId.hxx>

#include <RtpSession.hxx>
#include <RtpPacket.hxx>

#include "ResGwDevice.hxx"

#include "AssistDevice.hxx"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "pbx_globals.h"

namespace Vocal
{

class  AssistInvalidDevice:public AssistDevice
{

	public:
		friend class AssistNullDevice;
		
		AssistInvalidDevice( AssistNullDevice *agent, char *name, device_type_t type,  int deviceId, Sptr < Fifo < Sptr < SipProxyEvent > > > sessionQ);		
		~AssistInvalidDevice();

		void ProcessRTP();
		
		void provideQueryStatus();
		void provideDialToneStart();
		void provideDialToneStop();

		void provideRingStart();
		void provideRingStop();

		void provideLocalRingbackStart();
		void provideLocalRingbackStop();

		void provideRemoteRingbackStart();
		void provideRemoteRingbackStop();
		void provideBusyToneStart();
		void provideBusyToneStop();

		void provideFastBusyToneStart();
		void provideFastBusyToneStop();

		void provideCallWaitingBeepStart();
		void provideCallWaitingBeepStop();

		virtual unsigned char  reportDtmfDigit( );

	protected:
		virtual int openDeviceFile()  ;
		virtual void closeDeviceFile() ;
		
		bool emitDtmfSignal(char signal) ;

 	private:
		 	
};

}
#endif

