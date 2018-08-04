/*
* $Id: RegistrationManager.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "SipVia.hxx"
#include "SystemInfo.hxx"
#include "RegistrationManager.hxx"
#include "PbxConfiguration.hxx"
//#include "UaDevice.hxx"

using namespace Assist;

static const int mintag = 10000000;
static const int maxtagfix = 89999999;

///
RegistrationManager::RegistrationManager( Sptr < SipTransceiver > sipstack )
{
	urflag = false;
	sipStack = sipstack;
	//reg_on = PbxConfiguration::instance()->getRegisterOn(); 
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

void RegistrationManager::addRegistration(const string& phone_num, unsigned int cseq, bool reg)
{
	PbxConfiguration* config = PbxConfiguration::instance();
	// The first REGISTER message
	RegisterMsg registerMsg;

	// Set Request line
	/* modify by wuaiwu 2005/06/29 */
	Data reqUrlString = Data( string("sip:") + config->getRegisterServer() );
#if 0
	if(config->getSipTransport() == "TCP")
	{
		reqUrlString += ";transport=tcp;";
	}
#endif 

	Sptr< SipUrl > reqUrl = new SipUrl( reqUrlString );
	assert( reqUrl != 0 );
	SipRequestLine reqLine = registerMsg.getRequestLine();
	reqLine.setUrl( reqUrl );
	registerMsg.setRequestLine( reqLine );

	// Set From header
	const Data regFromUrlStr = Data(string("sip:") + config->getRegisterFrom());
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
	const Data regToUrlStr = Data( string("sip:") + config->getRegisterTo());
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
	Data viaBranch = "z9hG4bK" + registerMsg.computeBranch2();
	cpLog(LOG_DEBUG, "Branch:  %s", viaBranch.logData());
	sipvia.setBranch(viaBranch);
	registerMsg.removeVia();
	registerMsg.setVia( sipvia );

	/* modify by wuaiwu, 2005/11/11 */
	
	if (reg)
	{
		SipContact myContact;
		Sptr< SipUrl > contactUrl = new SipUrl;
		contactUrl->setUserValue( phone_num,  "phone" );
		contactUrl->setHost(Data( theSystem.gethostAddress() ) );
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
	{
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

	Registration registration( registerMsg );
	addRegistration( registration );
}

///
void RegistrationManager::addRegistration(const Registration& item)
{
    Registration* registration = new Registration(item);

    if ( 0 != registration )
        registrationList.push_back(registration);
}

///
void*
RegistrationManager::registrationThreadWrapper(void *regSession)
{
    assert( regSession );

    RegistrationManager* regManager = static_cast < RegistrationManager* > (regSession);
    regManager->registrationMain();
    return 0;
}


///
/* modify by wuaiwu, 2005/11/11 */
void  RegistrationManager::registrationMain()
{
	cpDebug(LOG_DEBUG, "RegistrationManager::registrationMain: starting registrationMain");
	
	cpDebug(LOG_DEBUG, "PID of RegistrationManager : %d" , getpid() );

	
	int delay = PbxConfiguration::instance()->getRegisterExpires() - 300;
	if (delay > 300) delay = 600;
	timeval start, now;
	while (1)
	{
		if(!PbxConfiguration::instance()->getRegisterOn() )
		{
			continue;
		}
		
		gettimeofday(&start, NULL);
		onceReg();
		sleep(delay);
		gettimeofday(&now, NULL);
		if (abs(now.tv_sec - start.tv_sec-delay) > 60)
		{
			urflag = true;
			onceReg(false);
			sleep(2);
			exit(0);
		}
	}

}

Registration*
RegistrationManager::findRegistration(const StatusMsg& statusMsg)
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

///

/* modify by wuaiwu, 2005/06/20 */
void
RegistrationManager::flushRegistrationList()
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

int
RegistrationManager::handleRegistrationResponse(const StatusMsg& statusMsg)
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
    	    cpLog(LOG_EMERG, "Send a Authorization REGISTER MESSAGE");
	    RegisterMsg registerMsg = registration->getNextRegistrationMsg();
	    sipStack->sendAsync(registerMsg);
	//    ret = false;   
    }

     /* end of modify */

    registrationMutex.unlock();

    if (urflag && scode == 200)
    {
    	 exit(0);
    }

    return scode;
}

///
void
RegistrationManager::startRegistration()
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

/* added by wuaiwu, 2005/11/11 */
void RegistrationManager::unRegister()
{
	urflag = true;
	onceReg(false);
	sleep(5);
	exit(0);
}


void RegistrationManager::onceReg(bool reg)
{

#if 0
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
	LocalDialMapType localDialMap = PbxConfiguration::instance()->getLocalDials();
	LocalDialMapType::iterator iter = localDialMap.begin();

	static string local =  PbxConfiguration::instance()->getLocalCountry() +
 	 			           PbxConfiguration::instance()->getLocalDistrict() +
 	 			           PbxConfiguration::instance()->getLocalDevice() + "0000";
	
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
	     	RegisterMsg registerMsg = registration->getNextRegistrationMsg();
		registration->setExpire();
		if ( (0 != sipStack && reg) || (!reg && registerMsg.getExpires().getData() == "0"))
	      	{
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
		{
			fifoEventIdMap.erase(pos);
		}

		if (reg)
		{
			pos = fifoEventIdMap.find(registration);
		        //only do this if there is no another registration message in the queue
		      if ( fifoEventIdMap.end() == pos )
		      {
		      		int delay = registration->getDelay()  / 4;
		            	FifoEventId eventId = registrationFifo.addDelayMs(registration, delay);
		            	fifoEventIdMap[registration] = eventId;
		      }
		}
             
        	registrationMutex.unlock();
	}

#endif 

}

/* end of added */

