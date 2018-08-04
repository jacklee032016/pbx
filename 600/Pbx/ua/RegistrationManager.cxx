/*
 * $Id: RegistrationManager.cxx,v 1.12 2007/06/18 11:11:17 lizhijie Exp $
 */

#include "SipVia.hxx"
#include "SystemInfo.hxx"
#include "RegistrationManager.hxx"
#include "UaConfiguration.hxx"
#include "AssistDevice.hxx"
#include "UaDevice.hxx"

using namespace Vocal;

static const int mintag = 10000000;
static const int maxtagfix = 89999999;

RegistrationManager::RegistrationManager( Sptr <SipTransceiver> _sipstack )
	:sipStack(_sipstack),
	urflag(false)
{
	exitFlag = false;
//	urflag = false;
//	sipStack = sipstack;
	//reg_on = UaConfiguration::instance()->getRegisterOn(); 
	cpDebug(LOG_DEBUG, "Starting Registration Mananger");
	//start the registration thread
	registrationThread.spawn(registrationThreadWrapper, this);
	cpDebug(LOG_DEBUG, "Spawning the registration thread");
      
}

RegistrationManager::~RegistrationManager()
{
	registrationThread.join();
	flushRegistrationList();
}

/* construct Registration (with a RegisterMsg) and add it into RegistrationList */
void RegistrationManager::addRegistration(const string& phone_num, unsigned int cseq, bool reg)
{
	UaConfiguration *config = UaConfiguration::instance();
	// The first REGISTER message
	RegisterMsg registerMsg;

	// Set Request line
	/* modify by wuaiwu 2005/06/29 */
	Data reqUrlString = Data( string("sip:") + config->getRegisterServer() + string(":") + config->getRegisterPort() );
#if 1
	if(config->getSipTransport() == "TCP")
	{
		reqUrlString += ";transport=tcp;";
	}
#endif 

	Sptr <SipUrl> reqUrl = new SipUrl( reqUrlString );
	assert( reqUrl != 0 );
	SipRequestLine reqLine = registerMsg.getRequestLine();
	reqLine.setUrl( reqUrl );
	registerMsg.setRequestLine( reqLine );

	Data hip=theSystem.gethostAddress(&reqUrl->getHost());
	
	// Set From header
#if 0/* lzj , 2007.03.06 */	
	const Data regFromUrlStr = Data(string("sip:") + config->getRegisterFrom());
#else
	const Data regFromUrlStr = Data(string("sip:") + config->getRegisterServer() );
#endif
	SipUrl regFromUrl( regFromUrlStr );
	SipFrom sipfrom = registerMsg.getFrom();   
	sipfrom.setDisplayName( config->getDisplayName() );
//	sipfrom.setUser( config->getUserName() );
	sipfrom.setUser( phone_num );
	sipfrom.setHost( regFromUrl.getHost() );
	sipfrom.setPort( regFromUrl.getPort() );
	
      /* added by wuaiwu, 2005/11/1 */
	struct timeval ctm;
	gettimeofday(&ctm, NULL);
	srand(ctm.tv_sec + ctm.tv_usec);
	int ntag = mintag + rand() % maxtagfix;
	Data ftag(ntag);
	sipfrom.setTag(ftag);
	/* end of added */
	registerMsg.setFrom( sipfrom );

	// Set To header
#if 0/* lzj , 2007.03.06 */	
	const Data regToUrlStr = Data( string("sip:") + config->getRegisterTo());
#else
	const Data regToUrlStr = Data( string("sip:")  + config->getRegisterServer() );
#endif
	SipUrl regToUrl( regToUrlStr );
	SipTo sipto = registerMsg.getTo();
	sipto.setDisplayName( config->getDisplayName() );
//	sipto.setUser( config->getUserName() );
	sipto.setUser( phone_num );
	sipto.setHost( regToUrl.getHost() );
	sipto.setPortData( regToUrl.getPort() );
	registerMsg.setTo( sipto );

	// Set Via header
	SipVia sipvia = registerMsg.getVia();
	sipvia.setPort( Data( config->getLocalSipPort() ).convertInt() );
	sipvia.setTransport(  Data(config->getSipTransport()) );
	sipvia.setHost(hip);
	Data viaBranch = "z9hG4bK" + registerMsg.computeBranch2();
	
	cpDebug(LOG_DEBUG, "Branch:  %s", viaBranch.logData());
	
	sipvia.setBranch(viaBranch);
	registerMsg.removeVia();
	registerMsg.setVia( sipvia );

	/* modify by wuaiwu, 2005/11/11 */
	
	if (reg)
	{
		SipContact myContact;
		Sptr <SipUrl> contactUrl = new SipUrl;
#if 0
		/* changed display name in contact header as 'corp name', 
		* modify SipUrl->setUserValue() as the same time. lzj, 2007.03.17 */
		contactUrl->setUserValue( phone_num,  "phone" );
#else
		contactUrl->setUserValue( phone_num,  config->getDisplayName());
#endif
		contactUrl->setHost( hip );
		contactUrl->setPort( config->getLocalSipPort() );
		if(config->getSipTransport() == "TCP")
		{
			contactUrl->setTransportParam( Data("tcp") );
		}

		myContact.setUrl( contactUrl );
		registerMsg.setNumContact( 0 );
		registerMsg.setContact(myContact);
	}
	else
	{/* deregistration */
		SipContact myContact("*");
		registerMsg.setNumContact( 0 );
		registerMsg.setContact(myContact);
		
	}
	
	SipExpires sipExpires;
	if (reg)
		sipExpires.setDelta( Data(config->getRegisterExpires() ));
	else
		sipExpires.setDelta(Data(0));

	TableCID::iterator titer = tableCallid.find(phone_num);
	if (titer == tableCallid.end())
	{
		tableCallid[phone_num] = new SipCallId();
	}

	registerMsg.setCallId(*(tableCallid[phone_num]));
	registerMsg.setExpires( sipExpires );
	 /* end of modify */

/*	change as following in order to avoid retransmit problem , lizhijie, 2007.03.06
	Registration registration( registerMsg );
*/	
	Registration registration( registerMsg, cseq );
	 
	addRegistration( registration );
}

void RegistrationManager::addRegistration(const Registration& item)
{
	Registration* registration = new Registration(item);

	if ( 0 != registration )
		registrationList.push_back(registration);
}

void *RegistrationManager::registrationThreadWrapper(void *regSession)
{
	assert( regSession );

	RegistrationManager* regManager = static_cast < RegistrationManager* > (regSession);
	regManager->registrationMain();
	return 0;
}

/* registration thread main loop : modify by wuaiwu, 2005/11/11 */
void  RegistrationManager::registrationMain()
{
	cpDebug(LOG_DEBUG, "RegistrationManager::registrationMain: starting registrationMain");
	cpDebug(LOG_DEBUG, "PID of RegistrationManager : %d" , getpid() );

	int delay = UaConfiguration::instance()->getRegisterExpires() - 300;
	if (delay > 300)
		delay = 600;
	
	timeval start, now;
		
	while (1)
	{
		if(!UaConfiguration::instance()->getRegisterOn() )
		{
			continue;
		}

		if(exitFlag== true )
		{
			cpLog(LOG_WARNING, "Begin to unregister" );
//			onceReg(false);
			registrationThread.exit();
			sleep(120);/* wait to exit by WorkerThread */
			/* return from this thread and wait exit by WorkerThread */
			continue;
		}
		
		gettimeofday(&start, NULL);
		onceReg();
		sleep(delay);
		gettimeofday(&now, NULL);
		if (abs(now.tv_sec - start.tv_sec-delay) > 60)
		{
//			urflag = true;
			onceReg(false);
			sleep(2);
//			exit(0);
		}
	}

}


/* find Registration with statusMsg feedback by Server */
Registration *RegistrationManager::findRegistration(const StatusMsg& statusMsg)
{
	Registration* registration = 0;
	registrationMutex.lock();
	
	RegistrationList::iterator iter = registrationList.begin();
	while ( iter != registrationList.end() )
	{
		RegisterMsg regMsg = (*iter)->getRegistrationMsg();
		if ( regMsg.computeCallLeg() == statusMsg.computeCallLeg() )
		{
			registration = (*iter);
			break;
		}
		iter++;
	}
	registrationMutex.unlock();

	return registration;
}

/* flush all expired registration  */
void RegistrationManager::flushRegistrationList()
{
	registrationMutex.lock();
	RegistrationList::iterator iter = registrationList.begin();
	while ( iter != registrationList.end() )
	{
		cpDebug(LOG_EMERG, "begin delete Register");
		if ( *iter != NULL  && (*iter)->isExpired())
		{
			cpDebug(LOG_EMERG, "deleted");
			delete (*iter);
			(*iter) = NULL;
		}
		++iter;
	}
	registrationList.clear();
	registrationMutex.unlock();
	cpDebug(LOG_DEBUG, "registrationList.size() = %d", registrationList.size());
}

/* executed in WorkerThread */
int RegistrationManager::handleRegistrationResponse(const StatusMsg& statusMsg)
{
	Registration* registration = findRegistration(statusMsg);

	if ( !registration )
		return -1;

	registrationMutex.lock();

	cpDebug(LOG_DEBUG, "RegistrationManager::handling response to a register message");
	FifoEventIdMap::iterator pos;
	pos = fifoEventIdMap.find(registration);
	if ( fifoEventIdMap.end() != pos )
	{
		FifoEventId eventId = fifoEventIdMap[registration];
		registrationFifo.cancel(eventId);
		fifoEventIdMap.erase(pos);
		cpDebug(LOG_EMERG, "--erase fifoEventIdMap element : %d", fifoEventIdMap.size());
	}

	cpDebug(LOG_DEBUG, "RegistrationManager::updating registration information");
	int delay = registration->updateRegistrationMsg(statusMsg);

	/* modify by wuaiwu, 2005/07/07 */
	int scode = registration->getStatusCode();

	if( scode == 100 )
	{
		const int DEFAULT_DELAY = 60000;   // 60 sec.
		delay = DEFAULT_DELAY;
	}

	// if (scode > 200 )
	// {
	//     ret = false;
	// }

	cpDebug(LOG_DEBUG, "The new delay is %d", delay);

	if (scode == 401 || scode == 407)
	{
		cpDebug(LOG_INFO, "Send a Authorization REGISTER MESSAGE when status code is %d", scode);
		RegisterMsg registerMsg = registration->getNextRegistrationMsg();
		sipStack->sendAsync(registerMsg);
		//    ret = false;   
	}
	/* end of modify */

	if (urflag && scode == 200)
	{/* unregister OK when SIP UA is shuting down */
		cpLog(LOG_DEBUG, "Unregistration 200-OK");
	}

	registrationMutex.unlock();

	return scode;
}

/* add Registration into FIFO */
void RegistrationManager::startRegistration()
{
	RegistrationList::iterator iter = registrationList.begin();

	while ( iter != registrationList.end() )
	{
		FifoEventId eventId = registrationFifo.addDelayMs((*iter), 0);
		fifoEventIdMap.erase((*iter));
		fifoEventIdMap[(*iter)] = eventId;
		iter++;
	}
}

/* called by DeviceThread when Signal is received */
void RegistrationManager::unRegister()
{
	cpLog(LOG_WARNING, "rx EXIT signal, then unregistering all users" );
	
	/* lock it in onceReg */
	exitFlag = true;
	urflag = true;

	onceReg(false);
	
	/* changed dalay from 5 to 60 for 9 registered phone number, 
	especially when authentication is required. lizhijie, 2007.03.28 */
	if(UaConfiguration::instance()->getRegisterOn())
		sleep(10);
	else
		sleep(1);
	
	exit(0);
#if 0
#endif	
}


void RegistrationManager::onceReg(bool reg)
{
	FifoEventIdMap::iterator pos;
	pos = fifoEventIdMap.begin();
	while ( fifoEventIdMap.end() != pos )
	{
		FifoEventId eventId = pos->second;
		registrationFifo.cancel(eventId);
		fifoEventIdMap.erase(pos);
		++pos;
	}
	
	flushRegistrationList();
	LocalDialMapType localDialMap = UaConfiguration::instance()->getLocalDials();
	LocalDialMapType::iterator iter = localDialMap.begin();

#if 0
	static string local =  UaConfiguration::instance()->getLocalCountry() +
 	 			           UaConfiguration::instance()->getLocalDistrict() +
 	 			           UaConfiguration::instance()->getLocalDevice() + "0000";
#else
	string local =  UaConfiguration::instance()->getLocalCountry() +
 	 			           UaConfiguration::instance()->getLocalDistrict() +
 	 			           UaConfiguration::instance()->getLocalDevice() + UaConfiguration::instance()->getConsolePhone();
#endif

	static unsigned int cseqnum = 100;
	++cseqnum;
	while ( iter != localDialMap.end() )
	{
		AssistDevice *dev = UaDevice::instance()->getAssistDeviceInfo( iter->second.c_str() );
		if( dev != 0)
		{
			addRegistration(string(dev->getPhoneNumber()), cseqnum, reg);
		}	
		iter++;
	}
	/* register Console Phone Number */
	addRegistration(local, cseqnum, reg);
	
	startRegistration();
	
	cpDebug(LOG_DEBUG, "RegistrationManager::registrationMain: getting next registration event");
	int objs = registrationFifo.size();
	while (objs > 0)
	{
		Registration* registration = registrationFifo.getNext();
		--objs;
		if ( 0 == registration )
	      		continue;
		
		registrationMutex.lock();
#if 0	/* lizhijie, 2007.03.28 un-registration operation when SIP UA is shotdown */	
	     	RegisterMsg registerMsg = registration->getNextRegistrationMsg();
#else
	     	RegisterMsg registerMsg;
		if(reg)
			registerMsg = registration->getNextRegistrationMsg();
		else
			registerMsg = registration->getNextRegistrationCancel();

#endif
		registration->setExpire();/* make it flush when next time come in this function */
		if ( (0 != sipStack && reg) || (!reg && registerMsg.getExpires().getData() == "0"))
	      	{/* send out register msg when reg or expired */
			cpDebug(LOG_DEBUG, "sipStack reg");
	      		SipCSeq mycseq = registerMsg.getCSeq();
			mycseq.setCSeq(cseqnum);
			registerMsg.setCSeq(mycseq);
	        	sipStack->sendAsync( registerMsg );
		}
		else
		{
			cpDebug(LOG_DEBUG, "sipStack == 0");
		}

		FifoEventIdMap::iterator pos;
		pos = fifoEventIdMap.find(registration);
		if ( fifoEventIdMap.end() != pos )
		{/* remove this registration from MAP */
			fifoEventIdMap.erase(pos);
		}

		if (reg)
		{
			pos = fifoEventIdMap.find(registration);

			/* only do this if there is no another registration message in the queue : last one has been removed */
			if ( fifoEventIdMap.end() == pos )
			{
				int delay = registration->getDelay()  / 4;
		            	FifoEventId eventId = registrationFifo.addDelayMs(registration, delay);
		            	fifoEventIdMap[registration] = eventId;
			}
		}
             
        	registrationMutex.unlock();
	}

}

