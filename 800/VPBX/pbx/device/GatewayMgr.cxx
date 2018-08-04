/*
* $Id: GatewayMgr.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"
#include "VMutex.h"
#include "LockHelper.hxx"

#include "GatewayMgr.hxx"

#include "DeviceMgr.hxx"
#include "EndPoint.hxx"

#include "PbxAgent.hxx"
#include "DialingValidator.hxx"

#include "PbxConfiguration.hxx"

#include <signal.h>

using namespace Assist;

#if 1
extern "C"
{
	void pbx_sighandlerexit(int signo, siginfo_t * info, void * val);
	int pbx_signal_register_handler(int sigkey, void (*sig_handler)(int, siginfo_t *, void *));
}

void pbx_sighandlerexit(int signo, siginfo_t *info, void *val)
{
//	cpLog(LOG_DEBUG,"Caught signal %d",signo);
	abort();
}

int pbx_signal_register_handler(int sigkey, void (*sig_handler)(int, siginfo_t *, void *))
{
	struct sigaction act;

	sigemptyset(&act.sa_mask);
	act.sa_flags=SA_SIGINFO;
	act.sa_sigaction=sig_handler;
	return sigaction(sigkey, &act, NULL);
}
#endif

GatewayMgr::GatewayMgr(PbxAgent *agent, Sptr < Fifo < Sptr <PbxEvent> > > _schedulerQueue)
	: PbxManager(agent, "GatewayMgr" ),
	mySchedulerQueue(_schedulerQueue ),
	mgrCount(0),
	epCount(0),
	audioCount(0)
{
	cpLog( LOG_DEBUG, "GatewayMgr " );

	myDialValidator = new DialingValidator();
	
	myHwQueue = new Fifo < Sptr <PbxEvent> >;
	assert( myHwQueue != 0 );

	myTaskQueue = new Fifo < Sptr <TaskEvent> >;
	assert( myTaskQueue != 0 );

	pbx_signal_register_handler(SIGINT, pbx_sighandlerexit); 
}

GatewayMgr::~GatewayMgr()
{
	cpLog(LOG_DEBUG, "Destorying.....");
}

void GatewayMgr::myMainLoop(void *params)
{/* no other operations are needed in Device Write Thread */
	return;
}

/* handle event come from both DeviceMgr and CallScheduler */
void GatewayMgr::myProcessEvent( Sptr <PbxEvent> event)
{
	assert ( event != 0 );

/*
	Sptr <PbxDeviceEvent > devEvent;
	devEvent.dynamicCast( event );
	assert ( devEvent != 0 );
*/

	Sptr <EndPoint> ep =  event->getEndPoint();
	assert(ep != 0);
/*
	Sptr <CtrlDevice> ctrl = ep->getCtrlDevice();
	ctrl->downProcessMain( devEvent);
*/	
	/* find state machine for this EndPoint and process  */
	ep->process( event);
}

/* in order to get better performance(mutex is only keep in CtrlDevice),
 * Audio Process is down in the level of CtrlDevice
*/
void GatewayMgr::processAudio()
{
	Sptr <DeviceMgr> mgr = 0;

	MgrIter iter = myMgrs.begin();
	
	while ( iter != myMgrs.end() )
	{
		mgr = iter->second;
		mgr->processAudio( );
		iter++;
	}
}

void GatewayMgr::registerDeviceMgr(const char *pluginName, Sptr <DeviceMgr> mgr )
{
	ExtMap phoneMaps = PbxConfiguration::instance()->getExtMap();
	assert ( mgr != 0 );

	mgr->setName(pluginName);
//	LockHelper lockHelper(myMutex);
//	myMutex.lock();
	if( strcmp(pluginName, "CONSOLE") == 0)
	{	
		cpLog(LOG_DEBUG, "%s is registered", pluginName);
		consoleMgr = mgr;
	}	

	myMgrs[mgrCount] = mgr;
	mgr->setIndex(mgrCount);
	mgrCount++;

	cpLog(LOG_DEBUG, "Register DeviceManager-%d : %s", mgrCount, mgr->getName() );
	mgr->setAccessCode( PbxConfiguration::instance()->getAccessCode(pluginName) );
	
	Sptr <CtrlDevice> ctrl = 0;
	int index = 0;

	while ( (ctrl = mgr->getNextDevice(index) ) != 0  )
	{
/*		
		ctrl->setExtPhoneNumber((*phoneIter).c_str());
		phoneIter++;
*/		
		cpLog(LOG_DEBUG, "add CtrlDevice-%d : %s", index+1, ctrl->getName() );
		Sptr <EndPoint> ep = 0;
		while( (ep = ctrl->getNextEP(ep) ) != 0)
		{
			registerEndPoint( ep);
			cpLog(LOG_DEBUG, "added EndPoint-%d : %s", ep->getIndex(), ep->getName() );
		}

		Sptr <AudioChannel> audio = 0;
		while( (audio = ctrl->getNextAudio( audio)) != 0)
		{
			registerAudio(audio);
			cpLog(LOG_DEBUG, "added AudioChannel-%d : %s", audio->getIndex(), audio->getName() );
		}
		++index;
	}

	/* assign ext phone number for every EXT EndPoint */
	/* EXT phone number is only used in PSTN and ISDN */
//	ExtMap::iterator iter = phoneMaps.find( mgr->getAccessCode() );
	ExtMap::iterator iter = phoneMaps.find( mgr->getName() );
	if(iter == phoneMaps.end() )
	{
		cpLog(LOG_ERR, "No Extension phone number defination for %s" , pluginName );
		return ;	
	}

	list <string>::const_iterator  phoneIter = iter->second.begin();
	index = 0;
	while ( (ctrl = mgr->getNextDevice(index) ) != 0  )
	{
		Sptr <EndPoint> ep = 0;
		while( (ep = ctrl->getNextEP(ep) ) != 0)
		{
			if(ep->checkExt() == true)
			{
				if( phoneIter== iter->second.end() )
				{
					cpLog(LOG_ERR,"No enough phone number defination for %s" , pluginName );
					assert(0);
				}
				ep->setPhoneNumber((*phoneIter).c_str());
				phoneIter++;
			}
//			cpLog(LOG_DEBUG, "added EndPoint-%d : %s", ep->getIndex(), ep->getName() );
		}
		index++;
	}

//	myMutex.unlock();
}

void GatewayMgr::registerEndPoint(Sptr <EndPoint> ep)
{
	assert ( ep != 0 );
//	LockHelper lockHelper(myMutex);

	myEps[epCount] = ep;
	ep->setIndex(epCount);
	
	epCount++;
//	cpLog(LOG_DEBUG, "add %d EndPoint %s", epCount, ep->getName() );
}

void GatewayMgr::registerAudio(Sptr <AudioChannel> audio)
{
	assert ( audio != 0 );
//	LockHelper lockHelper(myMutex);

	myAudios[audioCount] = audio;
	audio->setIndex(audioCount);

	audioCount++;
//	cpLog(LOG_DEBUG, "add %d AudioChannel %s", audioCount, audio->getName() );
}

/* just return the endpoint with this Phone Number, no matter whether it is busy 
* for internal extension dialing of ISDN and PSTN 
*/
Sptr <EndPoint>  GatewayMgr::findEPByNumber(const char *_number)
{
	Sptr <DeviceMgr> mgr = 0;
	const char *consoleCode = getConsoleAccessCode();

	assert(_number);
	cpLog(LOG_DEBUG, "get EP with phone number %s", _number);

	if( (strlen(_number) == 0 ) && consoleMgr!=0)
	{
		cpLog(LOG_DEBUG, "This is a call for Console");
		return consoleMgr->findFreeEP();
	}

	if(consoleMgr!=0 && consoleCode !=0 )
	{
		if( strncmp(_number,consoleCode , PbxConfiguration::instance()->getAccessLength() ) == 0) 
			return consoleMgr->findFreeEP();
	}
	
	int position = strlen(PbxConfiguration::instance()->getExtAccessCode() );

	EpIter iter = myEps.begin();
	while ( iter != myEps.end() )
	{
		Sptr <EndPoint> ep = iter->second;
		assert(ep != 0);
		if(ep->checkExt() == false)
		{
			iter++;
			continue;
		}	
		
		cpLog(LOG_DEBUG, "-----------%s", iter->second->getPhoneNumber());
		if ( strcmp((const char *) iter->second->getPhoneNumber(),  _number + position ) == 0 )
		{
			cpLog(LOG_DEBUG, "Found endpoint: %s", _number);
			assert(iter->second != 0);
			return iter->second;
		}	
		iter++;
	}

#if 1
	/* get DeviceMgr only with accessCode */
	mgr = findDeviceMgrByNumber( _number);
	if(mgr==NULL)
	{
		cpLog(LOG_ERR, "Can't Find DeviceMgr for Number: %s", _number);
		return NULL;
	}

	return mgr->lookupEndPointByNumber(_number);
#else
	cpLog(LOG_DEBUG, "Not Found endpoint: %s", _number);
	return NULL;	
#endif	
}

Sptr <EndPoint>  GatewayMgr::findEP(int index)
{
	assert ( index >= 0 );
	Sptr <EndPoint> ep = 0;

//	LockHelper lockHelper(myMutex);

	cpLog( LOG_DEBUG, "there are %d entries", myEps.size() );
	EpIter iter = myEps.find(index);

	if ( iter != myEps.end() )
	{
		cpLog( LOG_DEBUG, "found" );
		ep = iter->second;
	}
	else
	{
		cpLog( LOG_DEBUG, "not found" );
	}

	return ep;
}

Sptr <DeviceMgr>  GatewayMgr::findDeviceMgrByNumber(const char *_number)
{
	assert ( _number != 0 );
	Sptr <DeviceMgr> mgr = 0;

//	LockHelper lockHelper(myMutex);
	if( strlen(_number) == 0 )
		return consoleMgr;
	
	cpLog( LOG_DEBUG, "there are %d entries", myMgrs.size() );
	MgrIter iter = myMgrs.begin();

	while( iter != myMgrs.end() )
	{
		mgr = iter->second;
		cpLog( LOG_DEBUG, "found deviceMgr %s with phone number %s(Access Code length %d('%s') )" , 
			mgr->getName(), _number, PbxConfiguration::instance()->getAccessLength(), mgr->getAccessCode() );
		if(strncmp(_number, mgr->getAccessCode(), PbxConfiguration::instance()->getAccessLength()) == 0 )
		{
			cpLog( LOG_DEBUG, "found deviceMgr %s with phone number %s" , mgr->getName(), _number );
			return mgr;
		}	
		iter++;
	}

	return NULL;
}

Sptr <DeviceMgr>  GatewayMgr::findDeviceMgr(int index)
{
	assert ( index >= 0 );
	Sptr <DeviceMgr> mgr = 0;

//	LockHelper lockHelper(myMutex);

	cpLog( LOG_DEBUG, "there are %d entries", myMgrs.size() );
	MgrIter iter = myMgrs.find(index);

	if ( iter != myMgrs.end() )
	{
		cpLog( LOG_DEBUG, "found" );
		mgr = iter->second;
	}
	else
	{
		cpLog( LOG_DEBUG, "not found" );
	}

	return mgr;
}

Sptr <AudioChannel>  GatewayMgr::findAudio(int index)
{
	assert ( index < 0 );
	Sptr <AudioChannel> audio = 0;

//	LockHelper lockHelper(myMutex);

	cpLog( LOG_DEBUG, "there are %d entries", myAudios.size() );
	AudioIter iter = myAudios.find(index);

	if ( iter != myAudios.end() )
	{
		cpLog( LOG_DEBUG, "found" );
		audio = iter->second;
	}
	else
	{
		cpLog( LOG_DEBUG, "not found" );
	}

	return audio;
}

Sptr < Fifo < Sptr <PbxEvent> > > GatewayMgr::findDeviceQueue( int devIndex)
{
	Sptr <DeviceMgr> mgr = 0;

//	LockHelper lockHelper(myMutex);

	cpLog( LOG_DEBUG, "there are %d entries", myMgrs.size() );
	MgrIter iter = myMgrs.find(devIndex);

	if ( iter != myMgrs.end() )
	{
		cpLog( LOG_DEBUG, "found" );
		mgr = iter->second;
		return mgr->getQueue();
	}
	else
	{
		cpLog( LOG_DEBUG, "not found" );
		assert(0);
	}

	return 0;
}

/* Before enter into device write thread, all DeviceManagers and their device must be inited
* and registered in GatewayMgr, then threads of every DeviceManager must be execute
*
*/
int GatewayMgr::startup()
{
	/* start all deviceManager thread */
	Sptr <DeviceMgr> mgr = 0;

#if WITH_DEBUG
	debugInfo();
#endif

	MgrIter iter = myMgrs.begin();
	while ( iter != myMgrs.end() )
	{
		mgr = iter->second;
		mgr->execute( );
		iter++;
	}

	return 0;
}

#if WITH_DEBUG
void GatewayMgr::debugInfo()
{
	Sptr <DeviceMgr> mgr = 0;
	char *tmp;

	MgrIter iter = myMgrs.begin();
	while ( iter != myMgrs.end() )
	{
		mgr = iter->second;
		tmp = (char *)mgr->debugInfo();
		cpLog(LOG_DEBUG, "%s", tmp);
		free(tmp);
		iter++;
	}

	return ;
}
#endif

void GatewayMgr::sendHwCommand( Sptr <PbxEvent> event)
{
//	myMutex.lock();
	myHwQueue->add( event );
//	myMutex.unlock();
}


void  GatewayMgr::deviceThreadMainLoop ()
{
	//if( myHwQueue->size() > 0 )
	//{
		//myMutex.lock();
		cpLog( LOG_DEBUG, "DeviceCommandThread wait event..." );
		Sptr <PbxEvent> event =  myHwQueue->getNext();
		cpLog( LOG_DEBUG, "DeviceCommandThread get a event" );
		Sptr <EndPoint> ep = event->getEndPoint();
		assert(ep != 0 );
		Sptr <CtrlDevice> ctrl = ep->getCtrlDevice();

//		ctrl->sendHwCommand( event->getPrivate() );
		ctrl->sendHwCommand( event );
		//myMutex.unlock();
	//}
}

Sptr <DialingValidator> GatewayMgr::getDialingValidator()
{
	return myDialValidator;
}

Sptr <DeviceMgr>	GatewayMgr::getConsoleMgr()
{
	return consoleMgr;
}

const char *GatewayMgr::getConsoleAccessCode()
{
	if(consoleMgr == 0)
	{
		cpLog(LOG_ERR, "CONSOLE plugin is not loaded! Call can not be transfered into Console");
		return 0;
	}
	return consoleMgr->getAccessCode();
}

/* transfer call originated from this EP to console */
int GatewayMgr::callToConsole(Sptr <EndPoint> originateEndPoint)
{
	const char *number;
	unsigned int i;

	number = getConsoleAccessCode();
	if(number == 0)
		return 0;
	for(i=0; i<strlen(number); i++)
	{
		originateEndPoint->genDtmfEvent(number[i]);
	}
	
	return i;
}

void GatewayMgr::addTask( Sptr <TaskEvent> task)
{
//	myMutex.lock();
	myTaskQueue->add(task );
//	myMutex.unlock();
}

void  GatewayMgr::taskMainLoop ()
{
//	if( myQueue->size() > 0 )
	{
//		myMutex.lock();

		cpLog( LOG_DEBUG, "Waiting Task...." );
		Sptr <TaskEvent> task =  myTaskQueue->getNext();
		cpLog( LOG_DEBUG, "new task thread" );
//		myMutex.unlock();

		assert(task != 0);
		task->run();
	}
}

DialMethodsType GatewayMgr::checkAcceddCodeDialMethod(const string& externalNumber)
{
	cpLog(LOG_DEBUG,"=====external phone number =%s\n", externalNumber.c_str());
	int length = 0;

	DialMethodsType method = WRONG_DIAL;
	const char *dialstr = externalNumber.c_str();
	int accessLength = PbxConfiguration::instance()->getAccessLength();
	length = ((int)strlen(dialstr) >accessLength ) ? accessLength:strlen(dialstr);

#if 0	
	const char *consoleCode = PbxAgent::instance()->getGateway()->getConsoleAccessCode();
	if( consoleCode !=0 && length <=accessLength && strncmp(dialstr ,consoleCode , length) == 0  )
	{
		if(length == accessLength)
			return CONSOLE_DIAL;
		else
			return INCOMPLETE_DIAL;
	}
	
	int accessOk = 0;
	AccessCodeMap accessCodeMap = PbxConfiguration::instance()->getAccessCodeMap();
	
	AccessCodeMap::iterator itr = accessCodeMap.begin();
	while( itr != accessCodeMap.end() )
	{ 
		if(length<= accessLength &&  strncmp(dialstr , itr->second.c_str(), length ) == 0  )
		{
			accessOk = 1;
		}
		itr++;
	}
	if(accessOk == 0)
	{
		cpLog(LOG_DEBUG, "Dialing callee phone number %s with wrong access code", dialstr );
		return WRONG_DIAL;
	}
#endif

	Sptr <DeviceMgr> mgr = 0;

	MgrIter iter = myMgrs.begin();
	while( iter != myMgrs.end() )
	{
		mgr = iter->second;
		if(strncmp(dialstr, mgr->getAccessCode(), length) == 0 )
		{
			cpLog( LOG_DEBUG, "found deviceMgr %s with phone number %s" , mgr->getName(), dialstr );
			if(length < accessLength )
				return INCOMPLETE_DIAL;
			else if( mgr==consoleMgr || 
				(mgr->checkSecondDial() && PbxConfiguration::instance()->getFxoLineMode() == FXO_LINE_INDIRECT) )
				return mgr->getDialMethodType();
			else 
				return CONTINUE_CHECK_DIAL;
		}	
		iter++;
	}

	return method;
}

