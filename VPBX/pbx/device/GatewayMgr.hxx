#ifndef __GATEWAY_MGR_INSTANCE_HXX__
#define __GATEWAY_MGR_INSTANCE_HXX__
/*
* $Id: GatewayMgr.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "Sptr.hxx"
#include "Fifo.h"
#include "PbxEvent.hxx"
#include "TaskEvent.hxx"

#include "VMutex.h"

#include "PbxManager.hxx"

namespace Assist
{

class DeviceMgr;
class DialingValidator;

class GatewayMgr :public PbxManager
{
	public:

		GatewayMgr(PbxAgent *agent, Sptr < Fifo < Sptr <PbxEvent> > > _schedulerQueue);
		~GatewayMgr();
		
		void processAudio();

		void registerDeviceMgr(const char *pluginName, Sptr <DeviceMgr> mgr);
		
		/* find PbxEvent queue for the thread belone to this DeviceMgr */
		Sptr < Fifo < Sptr <PbxEvent> > > findDeviceQueue( int devIndex);

		Sptr <DeviceMgr>  findDeviceMgr(int index);

		/* called phone number , only used access code */
		Sptr <DeviceMgr>  findDeviceMgrByNumber(const char *_number);

		/* return a free EndPoint, else return null */
		Sptr <EndPoint>  findEPByNumber(const char *_number) ;
		
		Sptr <EndPoint>  findEP(int index);
		Sptr <AudioChannel>  findAudio(int index);

//		Sptr<DigitCollector> findDigitCollector(int epIndex);
		
		/* report event to callscheduler, mutex access by different thread */
		void sendHwCommand( Sptr <PbxEvent> event);
		void deviceThreadMainLoop();

		void addTask(Sptr <TaskEvent> task);
		void taskMainLoop();

		Sptr <DialingValidator>  getDialingValidator();

		Sptr <DeviceMgr>	getConsoleMgr();
		const char *getConsoleAccessCode();

		int callToConsole(Sptr <EndPoint> originateEndPoint);

		DialMethodsType checkAcceddCodeDialMethod(const string& externalNumber);
		
	protected:

		/* used by Device Write thread */
		void myMainLoop(void * params);
		void myProcessEvent(Sptr <PbxEvent> event);

		int startup();

	private:
		void registerEndPoint(Sptr <EndPoint> ep);

		void registerAudio(Sptr <AudioChannel> audio);

		void debugInfo();

		/* event queue of Scheduler, virtual Call Signal from all EP are pass to this queue */
		Sptr < Fifo < Sptr <PbxEvent> > > 			mySchedulerQueue;

		int 										mgrCount;
		typedef map < int, Sptr <DeviceMgr> > 		TableMgr;
		typedef TableMgr::iterator 					MgrIter;
		TableMgr 								myMgrs;

		int										epCount;
		typedef map < int, Sptr <EndPoint> > 		TableEp;
		typedef TableEp::iterator 					EpIter;
		TableEp 									myEps;

		int 										audioCount;
		typedef map < int, Sptr <AudioChannel> > 	TableAudio;
		typedef TableAudio::iterator				AudioIter;
		TableAudio 								myAudios;
		
//		VMutex		 							myMutex;
//		mutable VMutex 							myMutex;

		/* hold PbxdeviceEvent that write to device by DeviceCommandThread */
		Sptr < Fifo < Sptr <PbxEvent> > > 			myHwQueue;

		Sptr <DeviceMgr>							consoleMgr;

		Sptr <DialingValidator>						myDialValidator;

		Sptr < Fifo < Sptr <TaskEvent> > > 			myTaskQueue;

};
 
}

#endif

