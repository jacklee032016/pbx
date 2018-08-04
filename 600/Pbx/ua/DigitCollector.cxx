/*
 * $Id: DigitCollector.cxx,v 1.6 2007/03/16 19:33:56 lizhijie Exp $
 */

#include "global.h"
#include <cassert>
#include "DigitCollector.hxx"
#include "SystemInfo.hxx"
#include <Data.hxx>
#include <cpLog.h>
#include "UaDigitTimerEvent.hxx"
#include "UaDevice.hxx"
#include "UaConfiguration.hxx"
#include "AssistNullDevice.hxx"
using namespace Vocal;

// TODO  - replace tick count with time class and delay correct seconds
// TODO - implement dial string matching

const unsigned int DigitCollector::MAX_PREV_IN_URLS_SIZE = 10;
bool DigitCollector::bHasLoad = false;	
LocalDialMapType& DigitCollector::localDialMap = UaConfiguration::instance()->getLocalDials();
RegularContainer DigitCollector::Regulars;
SpeedDialMapType& DigitCollector::speedDialMap = UaConfiguration::instance()->getSpeedDials();
DialingPlanListType& DigitCollector::dialingPlanList = UaConfiguration::instance()->getDialPlans();
Sptr < Fifo < Sptr < SipProxyEvent > > > DigitCollector::callProcessingQueue;
OthsDialMapType& DigitCollector::recallDialMap = UaConfiguration::instance()->getOthsDials(0);
OthsDialMapType& DigitCollector::specialKeyDialMap = UaConfiguration::instance()->getOthsDials(1);
string DigitCollector::destHostIpAddr;
string DigitCollector::sipPortNum;
/* added 2007.03.09 */
string DigitCollector::proxyPortNum;
DigitTimeout* DigitCollector::interDigitTimeout;
	
DigitCollector::DigitCollector(int deviceId, Sptr < Fifo < Sptr < SipProxyEvent > > > cpq )
	:myDeviceId(deviceId)
{
	UaConfiguration* configuration = UaConfiguration::instance();
	myPending = new PendingDigitTimerEvent;
	myPending->deviceId = deviceId;
	myPending->type = INTER_DIGIT_TIME_OUT;
	myPending->eventId = 0;
	myPending->isTimeout = false;


    	dial = "";
    	dialMethod = INCOMPLETE_DIAL;
    	myDialedUrl = "";

	if(!bHasLoad)
	{

		interDigitTimeout = DigitTimeout::instance();

    		//DialingPlanListType dp = configuration->getDialPlans();
    		//DialingPlanListType::iterator dpIter = dp.begin();

#if 0	/* changed as following, 2007.03.09 */
    		string proxyServer = configuration->getProxyServer();
    		string::size_type colonPos = proxyServer.find( ":" );
    		destHostIpAddr = proxyServer.substr( 0, colonPos );
    		if ( colonPos < string::npos )
    		{
        		sipPortNum = proxyServer.substr( proxyServer.rfind( ":" ) + 1 );
    		}
    		else
    		{
        		sipPortNum = configuration->getLocalSipPort(); //configuration->getSipTransport();
    		}

		if(sipPortNum == "")
			sipPortNum = "5060";
#else
		destHostIpAddr = configuration->getProxyServer();
		proxyPortNum = configuration->getProxyPort();
		if(proxyPortNum == "")
			proxyPortNum = "9060";
		sipPortNum = configuration->getLocalSipPort();
		if(sipPortNum == "" )
			sipPortNum = "5060";
#endif
		#if 0
		while ( dpIter != dp.end() )
    		{
        		DialingPlan* dialPattern = new DialingPlan( *(*dpIter) );
        		addDialPlan( dialPattern );
        		dpIter++;
    		}

		SpeedDialMapType sd = configuration->getSpeedDials();
    		SpeedDialMapType::iterator sdIter = sd.begin();
    		while ( sdIter != sd.end() )
    		{
        		addSpeedDial( sdIter->first, sdIter->second );
        		sdIter++;
    		}

    		/* local pbx call map used the method of INTERNAL_IP_CALL */
     		LocalDialMapType localDial = configuration->getLocalDials();
     		LocalDialMapType::iterator iter = localDial.begin();
     		while ( iter != localDial.end() )
     		{
	 		addLocalDial( iter->first, iter->second );
	 		iter++;
     		}
		/* end of added by lizhijie */

		/* added by wuaiwu, get return call something, 2005/4/12 */
		/* for efficiency and restrain needless temporary objects 
		 * we should get the contianer length at first and use prev-++ instead of the back-++
	 	* notices by wuaiwu, 2005/4/12 */
		for (int i = 0; i < 2; ++i)
		{
			OthsDialMapType Dials = configuration->getOthsDials(i);
			OthsDialMapType::iterator iters = Dials.begin();
			OthsDialMapType::iterator ite_end = Dials.end();
			while (iters != ite_end)
			{
				addOthsDial(i,iters->first, iters->second);
				++iters;
			}
		}

		#endif 

		callProcessingQueue = cpq;
	
		setRegular();

		//flushDialPlan();

		bHasLoad = true;

	}	
}

void DigitCollector::reset()
{
	dial = "";
	dialMethod = INCOMPLETE_DIAL;
	myDialedUrl = "";
	interDigitTimeout->timerReset( myPending);
}

void DigitCollector::clear()
{
	dial = "";
	dialMethod = INCOMPLETE_DIAL;
	myDialedUrl = "";
	interDigitTimeout->timerCancel( myPending );
}


void DigitCollector::addDigit( char digit )
{
	isPbxDial = false;	
	//cancel the previous timeout timer
	interDigitTimeout->timerCancel(myPending);
	dial += digit;

	//start the inter-digit timeout timer for the next digit
	interDigitTimeout->timerStart( myPending);

	// Check if dialing complete
	dialMethod = findDialPlan();

	if((dialMethod== WRONG_DIAL) || (dialMethod!=INCOMPLETE_DIAL))
	{/* end of add digit and start call process : send INVITE msg */
		// Notify call processing
		Sptr < UaDigitTimerEvent > timerEvent = new UaDigitTimerEvent( callProcessingQueue );
		assert( timerEvent != 0 );

		timerEvent->callId = UaDevice::instance()->getCallId(myDeviceId);
		timerEvent->setDeviceId( myDeviceId );
		callProcessingQueue->add( timerEvent );
	}

}

void DigitCollector::addPbxDigit( char digit )
{
	 isPbxDial = true;
	 
	//cancel the previous timeout timer
	interDigitTimeout->timerCancel(myPending);
	dial += digit;

	//start the inter-digit timeout timer for the next digit
	interDigitTimeout->timerStart( myPending);

    	// Check if dialing complete
    	dialMethod = findDialPlan();
	
	if((dialMethod == WRONG_DIAL) || (dialMethod!=INCOMPLETE_DIAL))
    	{
       	 // Notify call processing
        	Sptr < UaDigitTimerEvent > timerEvent = new UaDigitTimerEvent( callProcessingQueue );
        	assert( timerEvent != 0 );

		timerEvent->callId = UaDevice::instance()->getCallId(myDeviceId);
		timerEvent->setDeviceId( myDeviceId );
		
        	callProcessingQueue->add( timerEvent );
    	}
}

void DigitCollector::setDialedUrl(const string& url)
{
    	dialMethod = URL_DIAL;
   	myDialedUrl = url;

    	// Notify call processing
    	Sptr <UaDigitTimerEvent> timerEvent  = new UaDigitTimerEvent( callProcessingQueue );
    	assert( timerEvent != 0 );

	timerEvent->callId = UaDevice::instance()->getCallId(myDeviceId);
	timerEvent->setDeviceId( myDeviceId );
	
    	callProcessingQueue->add( timerEvent );
}

/* added by lizhijie : check the dial method is INTERNAL_IP_DIAL is always first  */
bool DigitCollector::isValidPbxDial()
{
	bool bValid;

	//dialMethod = findDialPlan();

	if(dialMethod == INTERNAL_IP_DIAL || dialMethod == PSTN_DIAL)
		bValid = true;
	else
		bValid = false;
	
	if(!bValid)
		return false;
	else
	{
		interDigitTimeout->timerCancel( myPending );
		return true;
	}

}

bool DigitCollector::isValidDial()
{
	//dialMethod = findDialPlan();

	if (INCOMPLETE_DIAL == dialMethod || WRONG_DIAL == dialMethod)
	{
		return false;
	}
	else
	{
		interDigitTimeout->timerCancel( myPending );
		return true;
	}

}


bool DigitCollector::isDialTimeout()
{
	return myPending->isTimeout;
}


string DigitCollector::getDial()
{
	return isDialTimeout() ? dial + string("T") : dial;
}

const string& DigitCollector::getUrl()
{
	if (dialMethod == URL_DIAL)
		return myDialedUrl;

	switch ( dialMethod )
	{
		case SPEED_DIAL:
			getUrlFromSpeedDial();
			break;

		case SPECIAL_KEY_DIAL:
			getUrlFromSpecKeyDial();
			break;

		case NORMAL_DIAL:
			getUrlFromPhoneNumDial();
			break;

		/* uncomment this option , lizhijie 2005.02.09 */	
		case INTERNAL_IP_DIAL:
			getUrlFromLocalDial();//getUrlFromIpAddr();
			break;
		/* end of */		

		case CALL_RETURN_DIAL:
			getCallReturnUrl();
			break;

		case DIRECT_DIAL:
			getUrlFromDirectDial();
			cpDebug(LOG_DEBUG, "MYDIALEDURL: %s", myDialedUrl.c_str());
			break;

		case PSTN_DIAL:
			getPstnUrl();
			break;

		case INCOMPLETE_DIAL:
		case WRONG_DIAL:
			{}
			break;
			
		case URL_DIAL:
			{}
			break;
			
		default:
			{}
			 break;
	}

	return myDialedUrl;

}

DialMethodsType DigitCollector::checkDialMethod(const string& url)
{
	DialMethodsType method = WRONG_DIAL;

	const char *dialstr = url.c_str();
		
	for(RegularContainer::iterator iter = Regulars.begin(); iter != Regulars.end(); iter++)
	{

		if(REG_NOMATCH != regexec( &((*iter)->reg), dialstr, 0, NULL, 0))
		{
			if((*iter)->isFullMatch)
			{
				method = (*iter)->method;
				return method;
			}
		}	
		
	}

	return method;
}

/* url in format of '127.0.0.1' */
void DigitCollector:: getPstnUrl()
{
	string url; 
	string buf;

	if(!isPbxDial)
	{
		cpDebug(LOG_DEBUG, "!isPbxDial");
		buf = dial;
		url = "sip:" + buf + "@127.0.0.1:" + sipPortNum;

		#if 0

		if(buf[1] == '0')
		{
			if(buf[2] == '0')
				url = url+buf;
			else
				url = url+"000"+UaConfiguration::instance()->getLocalCountry()+buf.substr(2);
		}
		else
		{
			url = url+"000"+UaConfiguration::instance()->getLocalCountry()+UaConfiguration::instance()->getLocalDistrict()+\
				UaConfiguration::instance()->getLocalDevice()+buf.substr(1);
		}

		

		static string local = "000"+UaConfiguration::instance()->getLocalCountry()+UaConfiguration::instance()->getLocalDistrict()+UaConfiguration::instance()->getLocalDevice();

		if(url.size()>=local.size() && memcmp(local.c_str(), url.c_str(), local.size())==0)
			url = url.substr(0, url.size()-1);

		url += string("@");
		url += string("127.0.0.1");

		if (sipPortNum == "" )
		{
			url += ":5060";
		}
		else
		{
			url += string(":");
			url += UaConfiguration::instance()->getLocalSipPort();
		}
		#endif
	}	
	else
	{
		url = dial;
	}
	
	dialedType = dial_phone;

	myDialedUrl = url;
	#if 0
	string::size_type pos = 0;
	string filter("#*");
	//while ((pos = myDialedUrl.find_first_of(filter, pos)) != string::npos)myDialedUrl.erase(pos,1);
	#endif

	cpDebug(LOG_DEBUG, "PSTN DIAL: %s", url.c_str());

	//return url;
	
}


#if 0
void DigitCollector:: getUrlFromLocalDial()
{
	
	AssistDeviceInfo *dev = (AssistDeviceInfo *) checkHardwareForLocalDial(dial);
	if( dev == 0 )
	{
		myDialedUrl = "";
		return;
	}

	HardwareStatusType status = UaDevice::instance()->getHardwareStatus(dev->getMyDevice()->id);
	if(status== HARDWARE_UNAVAILABLE)
	{
		cpLog( LOG_ERR, "Hardware device with name of %s is busy now.", dev->getMyDevice()->name );
		myDialedUrl = "";
		return;
	}
	
	myDialedUrl= "sip:";
	myDialedUrl += dial;
	myDialedUrl += string("@");
	myDialedUrl += string("127.0.0.1");

	if (sipPortNum == "" )
	{
		myDialedUrl += ":5060";
	}
	else
	{
		myDialedUrl += string(":");
		myDialedUrl += UaConfiguration::instance()->getLocalSipPort ();
	}

	cpDebug(LOG_DEBUG , "DigitCollector url is returning for Local_Dial is '%s'", myDialedUrl.c_str());
	dialedType = dial_phone;

}

#endif

/* for call between local branch which is not pass PROXY server : url is always in format of "127.0.0.1' */
void  DigitCollector:: getUrlFromLocalDial()
{
	myDialedUrl = "";
	LocalDialMapType::iterator localDialIt = localDialMap.find(dial);
	if ( localDialIt == localDialMap.end() )
	{
		cpLog( LOG_ERR, "Local_Dial: %s  not found.", dial.c_str() );
		return ;
	}

	AssistDeviceInfo *dev = (AssistDeviceInfo *)UaDevice::instance()->getAssistDeviceInfo( localDialIt->second.c_str() );
	if(dev==0)
	{
		cpLog( LOG_ERR, "Hardware device with name of %s is not found.", localDialIt->second.c_str() );
		return ;
	}

	HardwareStatusType status = UaDevice::instance()->getHardwareStatus(dev->getDeviceId());
	
	if(status== HARDWARE_UNAVAILABLE)
	{
		cpLog( LOG_ERR, "Hardware device with name of %s is busy now.", dev->getMyDevice()->name );
		return;
	}
	
	myDialedUrl = "sip:";
	
	myDialedUrl += string(dev->getPhoneNumber());
	
	myDialedUrl += string("@");
	myDialedUrl += string("127.0.0.1:")+sipPortNum;
	
#if 0	
	if (sipPortNum == "" )
	{
		myDialedUrl += ":5060";
	}
	else
	{
		myDialedUrl += string(":");
		myDialedUrl += UaConfiguration::instance()->getLocalSipPort ();
	}
#endif	

	cpDebug(LOG_DEBUG , "DigitCollector url is returning for Local_Dial is '%s'", myDialedUrl.c_str());
	dialedType = dial_phone;

}

/*Normal Dial(2) : call the phone number which registered on PROXY Server */
void DigitCollector::getUrlFromPhoneNumDial()
{
	//get the current 3 digits of the IP address.
	if(destHostIpAddr == "")
	{
		myDialedUrl = "";
		return;
	}
	myDialedUrl = "sip:";
	myDialedUrl += dial;
	myDialedUrl += string("@");
	
#if 0/* lzj 2007.03.09 */
	myDialedUrl += destHostIpAddr+":"+sipPortNum;
#else
	myDialedUrl += destHostIpAddr+":"+proxyPortNum;
#endif
#if 0
	if (sipPortNum == "" )
	{
		myDialedUrl += ":5060";
	}
	else
	{
		myDialedUrl += string(":");
		myDialedUrl += sipPortNum;
	}
#endif

	cpDebug(LOG_DEBUG , "DigitCollector url is returning %s", myDialedUrl.c_str());

	dialedType = dial_phone;
}

/* which is not used now, lzj 2007.03.09 */
void DigitCollector::getUrlFromSpecKeyDial()
{
	string dialStr = dial;
	myDialedUrl = "sip:";
	OthsDialMapType::iterator ite = specialKeyDialMap.find(dialStr);
	if (ite == specialKeyDialMap.end())
	{
		myDialedUrl = "";
		return;	//url = "0000000";
	}
	else 
	{
	 	myDialedUrl += ite->second;
	}

	myDialedUrl += ":" + sipPortNum; // ":5060";

	cpDebug(LOG_DEBUG , "DigitCollector url is returning %s", myDialedUrl.c_str());

	dialedType = dial_phone;
	
}

void DigitCollector::getUrlFromSpeedDial()
{
	regex_t re;

	//get the current 3 digits of the IP address.
	string phoneNum = "";   // String retrieved from speedDialMap
	string userName = "";
	string password = "";
	string destination = "";
	string sipPort = "";
	myDialedUrl = "";

	SpeedDialMapType::iterator speedDialIt = speedDialMap.find(dial);
	if ( speedDialIt == speedDialMap.end() )
	{
		cpLog( LOG_ERR, "Speed_Dial: %s  not found.", dial.c_str() );
		return ;
	}
	else
	{
		phoneNum = speedDialIt->second;
	}

	if ( 0 == regcomp(&re, "[^0-9]", REG_EXTENDED) )
	{
		string::size_type colonPos1, colonPos2, atPos;
		/* format of 'user:passwd@ip_address:port ' */
		colonPos1 = phoneNum.find( ":" );
		colonPos2 = phoneNum.rfind( ":" );
		atPos = phoneNum.find( "@" );

		if( colonPos1 == string::npos )
		{
			// No ':'
			if( atPos == string::npos )
			{/* format of 'user' which is phone number. In this case call to user registered on PROXY server  */
				// No '@'
				// In this case, do we need to make sure that destination
				// is only [0-9]* ?
				userName = phoneNum;
				password = "";
				destination = "@" + destHostIpAddr;
#if 0
				sipPort = ":" + sipPortNum;
#else
				sipPort = ":" + proxyPortNum;
#endif
			}
			else
			{/* format of 'user@ip_address'. In this case direct call without Proxy Server */
				// We have an '@'
				userName = phoneNum.substr( 0, atPos );
				password = "";
				destination = phoneNum.substr( atPos );
				sipPort = ":"+sipPortNum; // ":5060";
			}
		}
		else  // We have at least one ':'
		{
			// In this case, we need an '@'
			if( atPos == string::npos )
			{/* error format */
				cpLog( LOG_ERR, "Error: No @ in Speed_Dial, exiting...\n" );
				return ;
			}

			if( colonPos1 != colonPos2 )
			{
				// We have a port & a password
				//		cpDebug(LOG_DEBUG, "We have a port & a password" );

				userName = phoneNum.substr( 0, colonPos1 );
				password = phoneNum.substr( colonPos1, atPos - colonPos1 );
				destination = phoneNum.substr( atPos, colonPos2 - atPos );
				sipPort = phoneNum.substr( colonPos2 );
			}
			else
			{
				// We have either a port, or a password
				if( colonPos1 < atPos )
				{
					// We have a password, but no port
					//		    cpDebug(LOG_DEBUG, "We have a password, but no port" );

					userName = phoneNum.substr( 0, colonPos1 );
					password = phoneNum.substr( colonPos1, atPos - colonPos1 );
					destination = phoneNum.substr( atPos );
					sipPort = ":"+sipPortNum;//":5060";
				}
				else
				{
					// We have a port, but no password
					//		    cpDebug(LOG_DEBUG, "We have a port, but no password" );

					userName = phoneNum.substr( 0, atPos );
					password = "";
					destination = phoneNum.substr( atPos, colonPos2 - atPos );
					sipPort = phoneNum.substr( colonPos2 );
				}
			}
		}
		
		// Set the SipUrl
		cpDebug(LOG_DEBUG, "userName = %s\npassword = %s\ndestinamtion = %s\nsipPort = %s\n",userName.c_str(), password.c_str(), destination.c_str(), sipPort.c_str());
		myDialedUrl = "sip:" + userName + password + destination + sipPort;

		if( atPos == string::npos )
		{
			dialedType = dial_phone;
		}
		else
		{
			dialedType = dial_ip;
		}
	}
	
	regfree(&re);

	cpDebug(LOG_DEBUG , "DigitCollector url is returning %s", myDialedUrl.c_str());
}


void DigitCollector::getCallReturnUrl()
{
     /*  modify  by wuaiwu for return call, 2005/4 /11 */
 
     OthsDialMapType::iterator iter_ = recallDialMap.find(dial);
     if (iter_ == recallDialMap.end())
     	{
     		myDialedUrl = "";
     		return ;//url = "0000000 not found dial";
     	}
     else
     	{
     		string temp = iter_->second;
		unsigned index = atoi(temp.c_str());
		
		//assert(index >= 0 && index < 10);

		if (prevInUrls.size() < index+1)
		{
			myDialedUrl = "";
			return ;//url = "0000000 no prev calls";
		}
		else
		{
			myDialedUrl = prevInUrls[index];
		}
     	}

	if ( myDialedUrl.find(dial_phone) != string::npos )
	{
            dialedType = dial_phone;
	}
        else
        {
             dialedType = dial_ip;
        }

	/* end of added */ 

}

void DigitCollector::getUrlFromDirectDial()
{
	string proxyServer = UaConfiguration::instance()->getProxyServer();
	if (proxyServer == "")
	{
		myDialedUrl = "";
		return;
	}

	string::size_type pos = 0; //;= myDialedUrl.find("#");
	string filter("#*");
	while ((pos = myDialedUrl.find_first_of(filter, pos)) != string::npos)
		myDialedUrl.erase(pos,1);
	
	static string local =  UaConfiguration::instance()->getLocalCountry() +
 	 		           UaConfiguration::instance()->getLocalDistrict() +
 	 		           UaConfiguration::instance()->getLocalDevice();

	if(myDialedUrl.size()>local.size() && memcmp(myDialedUrl.c_str(), local.c_str(), local.size())==0)
	{
		myDialedUrl = "sip:"+myDialedUrl+"@127.0.0.1:"+sipPortNum;
	}
	else	
	{
#if 0 /* lzj 2007.03.09 */	
		myDialedUrl = "sip:" + myDialedUrl + "@" + proxyServer;
		pos = proxyServer.find(":");
		if (pos == string::npos)
		{
			myDialedUrl += ":"+sipPortNum;
		}
#else
		myDialedUrl = "sip:" + myDialedUrl + "@" + destHostIpAddr + ":" + proxyPortNum ;
#endif
	}	
	
}


#if 0
///
void
DigitCollector::addSpeedDial(const string& key, const string& phoneNum)
{
    speedDialMap[key] = phoneNum;
}

void DigitCollector::addLocalDial(const string& key, const string& phoneNum)
{
	localDialMap[key] = phoneNum;
}

/// * added by wuaiwu, 2005/4/12 */

void DigitCollector::addOthsDial(int type, const string & key, const string & index)
{
      if (type == 0)
		recallDialMap[key] = index;
	else
		specialKeyDialMap[key] = index;
}
/// /* end of added */

///
void
DigitCollector::addDialPlan(const DialingPlan* dialingPlan)
{
    if ( dialingPlan == 0 )
    {
        return;
    }

    for( DialingPlanListType::iterator iter = dialingPlanList.begin();
	 iter != dialingPlanList.end(); iter++ )
    {
        const DialingPlan* pDialingPlan = *iter;

        if( pDialingPlan != 0  && *pDialingPlan == *dialingPlan )
        {
            dialingPlanList.erase( iter );
            break;
        }
    }

    dialingPlanList.push_back( dialingPlan );

}

#endif


#if 0
///
void
DigitCollector::flushDialPlan()
{
    DialingPlanListType::iterator iDialingPlanList = dialingPlanList.begin();

    while ( iDialingPlanList != dialingPlanList.end() )
    {
        const DialingPlan* dialingPlan = *iDialingPlanList;
        delete dialingPlan;

        ++iDialingPlanList;
    }
    dialingPlanList.clear();
}

#endif

void DigitCollector::setRegular()
{

    bool bExist;
    int iOrigLen;
    int iNowLen;
    regex_t re;
    Sptr<Regular>	regular;

	   	
    DialingPlanListType::iterator iDialingPlanList = dialingPlanList.begin();

    while ( iDialingPlanList != dialingPlanList.end() )
    {
    	 const DialingPlan* dialingPlan = *iDialingPlanList;

        if ( !dialingPlan )
            continue;

        char *pattern = (char *)dialingPlan->getDialPattern().c_str();
	 iOrigLen = strlen(pattern);
	 iNowLen = iOrigLen;
	 
	while(iNowLen)
	{
		bExist = false;
		if ( 0 != regcomp(&re, pattern, REG_EXTENDED) )
        	{
        		//the full sting is not a corret regular expression, so ignore it
           		if(iNowLen == iOrigLen)
				break;
        	}
		else
		{
			//check if it is an exist regular, if so, ignore it 	
		    	for(RegularContainer::iterator iter=Regulars.begin(); iter!=Regulars.end();  iter++)
    		   	{
        	   		Sptr<Regular> regular= (*iter);
				regex_t reg = regular->reg;	
		       	if(!memcmp(&reg, &re, sizeof(regex_t)))
		       	{
					regfree(&re);
					bExist = true;
					break;
		       	}	
		    	}

			if(bExist)
				continue;
				
			regular = new Regular;
			regular->reg = re;
			regular->method = dialingPlan->getDialMethod();
			if(iNowLen == iOrigLen)
				regular->isFullMatch = true;
			else
				regular->isFullMatch = false;
			Regulars.push_back(regular);
			
		}
      		pattern[iNowLen-1]=0;
		iNowLen--;
	}
	iDialingPlanList++;
    }

}

///

DialMethodsType DigitCollector::findDialPlan()
{
	//0 no math, 1 has part match, 2 full match
	int iFlag = 0; 
	regmatch_t pmatch[3];

	DialMethodsType method = WRONG_DIAL;

	const char *dialstr = dial.c_str();

	int timr=strlen(dialstr) -1; //shreshold of part dial	
	assert(timr >= 0);
	int tmp=timr;	

	for(RegularContainer::iterator iter = Regulars.begin(); iter != Regulars.end(); iter++)
	{
		if(isPbxDial && ((*iter)->method != INTERNAL_IP_DIAL && (*iter)->method != PSTN_DIAL))
			continue;	
		if(REG_NOMATCH != regexec( &((*iter)->reg), dialstr, (*iter)->reg.re_nsub+1,pmatch, 0))
		{
			if((*iter)->isFullMatch)
			{
				iFlag = 2;
				method = (*iter)->method;
				break;
			}

			if(pmatch[0].rm_so == 0)
			{
				int len = pmatch[0].rm_eo - pmatch[0].rm_so;
					if(tmp < len)tmp  = len;		
			}

		}
	}

	if(tmp > timr)
		iFlag = 1;

	switch(iFlag)
	{
		case 1:
			if(dial.length()<=30)
				method = INCOMPLETE_DIAL;
			break;
		case 0:
		case 2:
			break;
			
		default:
			break;
	}

	switch ( method )
	{
		case DIRECT_DIAL:
		{
			cpDebug(LOG_DEBUG, "DRIECT_DIAL");
			if (pmatch[0].rm_so != 0 || pmatch[0].rm_eo > 16)
			{
				cpDebug(LOG_DEBUG, "Pattern matched but not vaild");
				method = WRONG_DIAL;
			}
			else
			{
				//cpDebug(LOG_DEBUG, "pmatch[0].rm_eo = %d", pmatch[0].rm_eo);
				myDialedUrl = dial;
				switch (pmatch[0].rm_eo)
				{                         
					case TOWNSMAN:
						myDialedUrl = UaConfiguration::instance()->getLocalDistrict() +myDialedUrl;
						myDialedUrl = UaConfiguration::instance()->getLocalCountry() + myDialedUrl;
						//cpDebug(LOG_DEBUG, "REAL: %s", dial.c_str());
						//printf("_________townsman: %s : %d\n", myDialedUrl.c_str(), TOWNSMAN);
						break;

					case HOMELAND_USER1:
					case HOMELAND_USER2:
					case HOMELAND_USER3:
					case HOMELAND_USER4:
						myDialedUrl = UaConfiguration::instance()->getLocalCountry() + myDialedUrl;
						//printf("_________homelanduser: %s : %d/%d\n", myDialedUrl.c_str(), HOMELAND_USER1,HOMELAND_USER2);
						//cpDebug(LOG_DEBUG, "REAL: %s", dial.c_str());
						break;

					default:
						cpDebug(LOG_DEBUG, "Pattern matched but not vaild-2");
						break;
				}
				cpDebug(LOG_DEBUG, "REAL: %s", myDialedUrl.c_str());

			}
		}
		break;
			
		case SPEED_DIAL:
		{
			cpDebug(LOG_DEBUG, "SPEED_DIAL" );
		}
		break;

		case SPECIAL_KEY_DIAL:        
		{
			cpDebug(LOG_DEBUG, "SPECIAL_KEY_DIAL" );           
		}               
		break;

		case NORMAL_DIAL:
		{
			cpDebug(LOG_DEBUG, "NORMAL_DIAL" );
		}
		break;

		case INTERNAL_IP_DIAL:
		{
			cpDebug(LOG_DEBUG, "INTERNAL_IP_DIAL" );
		}
		break;

		/* added by wuaiwu, 2005/4/12 */
		case CALL_RETURN_DIAL:
		{
			cpDebug(LOG_DEBUG, "CALL_RETURN_DIAL");
		}
		break;

		case PSTN_DIAL:
		{
			cpDebug(LOG_DEBUG, "PSTN_DIAL");
		}
		break;

		case INCOMPLETE_DIAL:
		{
			cpDebug(LOG_DEBUG, "INCOMPLETE_DIAL" );
		}
		break;

		default:
		{
			cpDebug(LOG_DEBUG, "WRONG_DIAL" );
		}
	}    /* switch */

	return method;
}

//
void DigitCollector::addIncomingUrl( const string& url )
{
	if ( prevInUrls.size() >= MAX_PREV_IN_URLS_SIZE )
	{
		prevInUrls.pop_back();
	}
	prevInUrls.insert(prevInUrls.begin(), url);
}

///constructor for the dialing plan
DialingPlan::DialingPlan(string pattern, DialMethodsType method)
{
	dialPattern = pattern;
	dialMethod = method;
}

DialingPlan::DialingPlan(const DialingPlan& src)
{
	dialPattern = src.dialPattern;
	dialMethod = src.dialMethod;
}

///equality operator for DialingPlan.
///returns true if the dialPattern are equivalent
bool DialingPlan::operator ==(const DialingPlan& dialPlan) const
{
	return dialPattern == dialPlan.dialPattern;
}

void DialingPlan::operator =(const DialingPlan& src)
{
	dialPattern = src.dialPattern;
	dialMethod = src.dialMethod;
}




#if 0


/*$Id: DigitCollector.cxx,v 1.6 2007/03/16 19:33:56 lizhijie Exp $*/
#include "global.h"
#include <cassert>

#include "DigitCollector.hxx"
#include "SystemInfo.hxx"
#include <Data.hxx>
#include <cpLog.h>
#include "UaDigitTimerEvent.hxx"
#include "UaDevice.hxx"
#include "UaConfiguration.hxx"
#include "AssistNullDevice.hxx"
using namespace Vocal;

// TODO  - replace tick count with time class and delay correct seconds
// TODO - implement dial string matching

#if 0 //modified by lijie 2005-07-05
const unsigned int DigitCollector::MAX_PREV_IN_URLS_SIZE = 10;

DigitCollector::DigitCollector(int deviceId, Sptr < Fifo < Sptr < SipProxyEvent > > > cpq )
	:myDeviceId(deviceId)
{
    UaConfiguration* configuration = UaConfiguration::instance();
	myPending = new PendingDigitTimerEvent;
	myPending->deviceId = deviceId;
	myPending->type = INTER_DIGIT_TIME_OUT;
	myPending->eventId = 0;
	myPending->isTimeout = false;

    string proxyServer = configuration->getProxyServer();
    string::size_type colonPos = proxyServer.find( ":" );
    destHostIpAddr = proxyServer.substr( 0, colonPos );
    if ( colonPos < string::npos )
    {
        sipPortNum = proxyServer.substr( proxyServer.rfind( ":" ) + 1 );
    }
    else
    {
        sipPortNum = "5060";
    }

    dial = "";
    isNewDigit = false;
    dialMethod = INCOMPLETE_DIAL;
    myDialedUrl = "";

#if 0
    //initiate the inter-digit timeout timer
    interDigitTimeout = Sptr < DigitTimeout > (new DigitTimeout( cpq ));
    assert( interDigitTimeout != 0 );

    interDigitTimeout->setInitTimeout( configuration->getInitialDigitTimeout() );
    interDigitTimeout->setInterDigitTimeout( configuration->getInterDigitTimeout() );
#endif
		interDigitTimeout = DigitTimeout::instance();
		
/*add by Tangbiao 2005-4-5*/		
    interDigitTimeout->setInitTimeout( configuration->getInitialDigitTimeout() );
    interDigitTimeout->setInterDigitTimeout( configuration->getInterDigitTimeout() );
/*add by Tangbiao 2005-4-5*/	


/*add by Tangbiao 2005-4-4*/
//UaConfiguration::instance()->lock_config(); 
/*add by  Tangbiao 2005-4-4*/

    DialingPlanListType dp = configuration->getDialPlans();
    DialingPlanListType::iterator dpIter = dp.begin();
    while ( dpIter != dp.end() )
    {
        DialingPlan* dialPattern = new DialingPlan( *(*dpIter) );
        addDialPlan( dialPattern );
        dpIter++;
    }

    SpeedDialMapType sd = configuration->getSpeedDials();
    SpeedDialMapType::iterator sdIter = sd.begin();
    while ( sdIter != sd.end() )
    {
        addSpeedDial( sdIter->first, sdIter->second );
        sdIter++;
    }

	/* local pbx call map used the method of INTERNAL_IP_CALL */
	LocalDialMapType localDial = configuration->getLocalDials();
	LocalDialMapType::iterator iter = localDial.begin();
	while ( iter != localDial.end() )
	{
		addLocalDial( iter->first, iter->second );
		iter++;
	}
	/* end of added by lizhijie */


	/* added by wuaiwu, get return call something, 2005/4/12 */
	/* for efficiency and restrain needless temporary objects 
	 * we should get the contianer length at first and use prev-++ instead of the back-++
	 * notices by wuaiwu, 2005/4/12 */

	for (int i = 0; i < 2; ++i)
	{
		OthsDialMapType Dials = configuration->getOthsDials(i);
		OthsDialMapType::iterator iters = Dials.begin();
		OthsDialMapType::iterator ite_end = Dials.end();
		while (iters != ite_end)
		{
			addOthsDial(i,iters->first, iters->second);
			++iters;
		}
	}
	
	/* end of added in ctor */


/*add by Tangbiao 2005-4-4*/
//UaConfiguration::instance()->unlock_config(); 
/*add by  Tangbiao 2005-4-4*/

    callProcessingQueue = cpq;
}

void
DigitCollector::reset()
{
   
     
    dial = "";
    isNewDigit = false;
    dialMethod = INCOMPLETE_DIAL;
    myDialedUrl = "";
/*add by Tangbiao 2005-4-8*/
    UaConfiguration* configuration = UaConfiguration::instance();		
    interDigitTimeout->setInitTimeout( configuration->getInitialDigitTimeout() );
    interDigitTimeout->setInterDigitTimeout( configuration->getInterDigitTimeout() );
/*add by Tangbiao 2005-4-8*/	
	
	//reset the inter-digit timeout timer
	interDigitTimeout->timerReset( myPending);
}


void
DigitCollector::clear()
{

    dial = "";
    isNewDigit = false;
    dialMethod = INCOMPLETE_DIAL;
    myDialedUrl = "";
/*add by Tangbiao 2005-4-8*/	
    UaConfiguration* configuration = UaConfiguration::instance();	
    interDigitTimeout->setInitTimeout( configuration->getInitialDigitTimeout() );
    interDigitTimeout->setInterDigitTimeout( configuration->getInterDigitTimeout() );
/*add by Tangbiao 2005-4-8*/
	//reset the inter-digit timeout timer
	interDigitTimeout->timerCancel( myPending );
}


void
DigitCollector::addDigit( char digit )
{
	//cancel the previous timeout timer
	interDigitTimeout->timerCancel(myPending);

    dial += digit;
    isNewDigit = true;
    UaConfiguration* configuration = UaConfiguration::instance();
    interDigitTimeout->setInitTimeout( configuration->getInitialDigitTimeout() );
    interDigitTimeout->setInterDigitTimeout( configuration->getInterDigitTimeout() );
	//start the inter-digit timeout timer for the next digit
	interDigitTimeout->timerStart( myPending);

    // Check if dialing complete
    DialMethodsType stage = findDialPlan();

    if ( !( stage == INCOMPLETE_DIAL || stage == WRONG_DIAL ) )
    {
        // Notify call processing
        Sptr < UaDigitTimerEvent > timerEvent = new UaDigitTimerEvent( callProcessingQueue );
        assert( timerEvent != 0 );

		timerEvent->callId = UaDevice::instance()->getCallId(myDeviceId);
		timerEvent->setDeviceId( myDeviceId );
		
        callProcessingQueue->add( timerEvent );
    }
}

void
DigitCollector::addPbxDigit( char digit )
{
	//cancel the previous timeout timer
	interDigitTimeout->timerCancel(myPending);

    dial += digit;
    isNewDigit = true;
    UaConfiguration* configuration = UaConfiguration::instance();
    interDigitTimeout->setInitTimeout( configuration->getInitialDigitTimeout() );
    interDigitTimeout->setInterDigitTimeout( configuration->getInterDigitTimeout() );
	//start the inter-digit timeout timer for the next digit
	interDigitTimeout->timerStart( myPending);

    // Check if dialing complete
    DialMethodsType stage = findDialPlan();

    if ( !( stage == INCOMPLETE_DIAL || stage == WRONG_DIAL ) )
    {
        // Notify call processing
        Sptr < UaDigitTimerEvent > timerEvent = new UaDigitTimerEvent( callProcessingQueue );
        assert( timerEvent != 0 );

		timerEvent->callId = UaDevice::instance()->getCallId(myDeviceId);
		timerEvent->setDeviceId( myDeviceId );
		
        callProcessingQueue->add( timerEvent );
    }
}

void
DigitCollector::setDialedUrl(const string& url)
{
    dialMethod = URL_DIAL;
    myDialedUrl = url;

    // Notify call processing
    Sptr < UaDigitTimerEvent > timerEvent 
	= new UaDigitTimerEvent( callProcessingQueue );
    assert( timerEvent != 0 );

	timerEvent->callId = UaDevice::instance()->getCallId(myDeviceId);
	timerEvent->setDeviceId( myDeviceId );
	
    callProcessingQueue->add( timerEvent );
}

/* added by lizhijie : check the dial method is INTERNAL_IP_DIAL is always first  */
bool DigitCollector::isValidPbxDial()
{
	dialMethod = findDialPlan();
	if (INCOMPLETE_DIAL == dialMethod || WRONG_DIAL == dialMethod)
	{
		return false;
	}
	else if(INTERNAL_IP_DIAL != dialMethod )
		return false;
	else
	{
		interDigitTimeout->timerCancel( myPending );
		return true;
	}
}

bool
DigitCollector::isValidDial()
{

//    if ( !isNewDigit ) return false;

	dialMethod = findDialPlan();
//    isNewDigit = false;

	if (INCOMPLETE_DIAL == dialMethod || WRONG_DIAL == dialMethod)
	{
		return false;
	}
	else
	{
		interDigitTimeout->timerCancel( myPending );
		return true;
	}

}


bool DigitCollector::isDialTimeout()
{
#if  0
    return interDigitTimeout->isInterDigitTimeout();
#endif


        /*add by Tangbiao 2005-4-5*/
          //reload_AllPlan();
        /*add by Tangbiao 2005-4-5*/
	return myPending->isTimeout;
}


string
DigitCollector::getDial()
{
    return isDialTimeout() ? dial + string("T") : dial;
}


string
DigitCollector::getUrl()
{
    string url;
    switch ( dialMethod )
    {
    case SPEED_DIAL:
	url = getUrlFromSpeedDial();
	break;
    case SPECIAL_KEY_DIAL:
	url = getUrlFromSpecKeyDial();
	break;
    case NORMAL_DIAL:
	url = getUrlFromPhoneNumDial();
	break;
	
/* uncomment this option , lizhijie 2005.02.09 */	
	case INTERNAL_IP_DIAL:
		url = getUrlFromLocalDial();//getUrlFromIpAddr();
		break;
/* end of */		

    case CALL_RETURN_DIAL:
	url = getCallReturnUrl();
	break;
    case INCOMPLETE_DIAL:
    case WRONG_DIAL:
	url = "";
	break;
    case URL_DIAL:
	url = myDialedUrl;
	break;
    default:
	url = "";
        break;
    }
    return url;
}

/* added by lizhijie, 2005.02.16 */
/* return true : device corrsponding the dial_phone_number is availabled */
AssistDevice*  DigitCollector::checkHardwareForLocalDial( string dial_phone_number )
{

        /*add by Tangbiao 2005-4-4*/
        reload_AllPlan();
        /*add by Tangbiao 2005-4-4*/
	LocalDialMapType::iterator localDialIt = localDialMap.find(dial_phone_number);
	if ( localDialIt == localDialMap.end() )
	{
		cpLog( LOG_ERR, "Local_Dial: %s  not found.", dial_phone_number.c_str() );
		return 0;
	}

	AssistDeviceInfo *dev = (AssistDeviceInfo *)UaDevice::instance()->getAssistDeviceInfo( localDialIt->second.c_str() );
	if(dev==0)
	{
		cpLog( LOG_ERR, "Hardware device with name of %s is not found.", localDialIt->second.c_str() );
		return 0;
	}
#if 0
	HardwareStatusType status = UaDevice::instance()->getHardwareStatus(dev->getMyDevice()->id);
	if(status== HARDWARE_UNAVAILABLE)
	{
		cpLog( LOG_ERR, "Hardware device with name of %s is busy now.", localDialIt->second.c_str() );
		return 0;
	}
#endif
	return dev;
}

string DigitCollector::getPhoneNumber( int deviceId )
{
         /*add by Tangbiao 2005-4-4*/
               reload_AllPlan();
         /*add by Tangbiao 2005-4-4*/
	AssistDeviceInfo *dev = (AssistDeviceInfo *)UaDevice::instance()->getAssistDevice( deviceId );
	if(dev !=0)
	{
	                
		LocalDialMapType::iterator iter = localDialMap.begin();
		while ( iter != localDialMap.end() )
		{
			if( strcmp( dev->getMyDevice()->name , iter->second.c_str() )==0 ) 
			{
				return iter->first;
			}
			iter++;
		}
	}
	
	cpLog( LOG_ERR, "Hardware device with deviceID as %d is not found. Check your config file carefully", deviceId );
	return UaConfiguration::instance()->getUserName();
}

string DigitCollector:: getUrlFromLocalDial()
{
	
	AssistDeviceInfo *dev = (AssistDeviceInfo *) checkHardwareForLocalDial(dial);
	if( dev == 0 )
		return string("");

	HardwareStatusType status = UaDevice::instance()->getHardwareStatus(dev->getMyDevice()->id);
	if(status== HARDWARE_UNAVAILABLE)
	{
		cpLog( LOG_ERR, "Hardware device with name of %s is busy now.", dev->getMyDevice()->name );
		return string("");
	}
	
	string url = "sip:";
	url += dial;
	url += string("@");
	url += string("127.0.0.1");

	if (sipPortNum == "" )
	{
		url += ":5060";
	}
	else
	{
		url += string(":");
		url += UaConfiguration::instance()->getLocalSipPort ();
	}

	cpDebug(LOG_DEBUG , "DigitCollector url is returning for Local_Dial is '%s'", url.c_str());
	dialedType = dial_phone;

	return url;
}
/* end of added */

string
DigitCollector::getUrlFromPhoneNumDial()
{
    //get the current 3 digits of the IP address.
    string url = "sip:";
    url += dial;
    url += string("@");

    url += destHostIpAddr;

    if (sipPortNum == "" )
    {
        url += ":5060";
    }
    else
    {
        url += string(":");
        url += sipPortNum;
    }

    cpDebug(LOG_DEBUG , "DigitCollector url is returning %s", url.c_str());

    dialedType = dial_phone;

    return url;
}

string
DigitCollector::getUrlFromSpecKeyDial()
{
   
     string dialStr = dial;

    //if ( !dialStr.empty() )
     //   dialStr.resize( dialStr.length() - 1 );

    string url("sip:");
    OthsDialMapType::iterator ite = specialKeyDialMap.find(dialStr);
    if (ite == specialKeyDialMap.end())
    {
    		url = "0000000";
    }
    else 
    {
	 	url += ite->second;
    }

    url += ":5060";

   // string url = "sip:";
    //url += dialStr;
   // url += string("@");
   // url += destHostIpAddr;

   // if (sipPortNum == "" )
  //  {
 //       url += ":5060";
  //  }
  //  else
  //  {
   //     url += string(":");
   //     url += sipPortNum;
   // }

    cpDebug(LOG_DEBUG , "DigitCollector url is returning %s", url.c_str());

    dialedType = dial_phone;

    return url;
}

string
DigitCollector::getUrlFromSpeedDial()
{
    regex_t re;

    //get the current 3 digits of the IP address.
    string phoneNum = "";   // String retrieved from speedDialMap
    string url = "";        // url that we return
    string userName = "";
    string password = "";
    string destination = "";
    string sipPort = "";
    
    
/*add by Tangbiao 2005-4-4*/
reload_AllPlan();
/*add by Tangbiao 2005-4-4*/

    SpeedDialMapType::iterator speedDialIt = speedDialMap.find(dial);
    if ( speedDialIt == speedDialMap.end() )
    {
	cpLog( LOG_ERR, "Speed_Dial: %s  not found.", dial.c_str() );
        phoneNum = "0000000";
#if 0		
		/* added this line by lizhijie, 2005.03.08 . only validate speed dial item is found, the opInviteUrl can be continued*/
		return 0;
#endif
    }
    else
    {
        phoneNum = speedDialIt->second;
    }

    if ( 0 == regcomp(&re, "[^0-9]", REG_EXTENDED) )
    {
	string::size_type colonPos1, colonPos2, atPos;

	colonPos1 = phoneNum.find( ":" );
	colonPos2 = phoneNum.rfind( ":" );
	atPos = phoneNum.find( "@" );
    
	if( colonPos1 == string::npos )
	{
	    // No ':'

	    if( atPos == string::npos )
	    {
		// No '@'
		// In this case, do we need to make sure that destination
		// is only [0-9]* ?
		userName = phoneNum;
		password = "";
		destination = "@" + destHostIpAddr;
                sipPort = ":" + sipPortNum;

	    }
	    else
	    {
		// We have an '@'
		userName = phoneNum.substr( 0, atPos );
		password = "";
		destination = phoneNum.substr( atPos );
		sipPort = ":5060";
	    }
	}
	else  // We have at least one ':'
	{
	    // In this case, we need an '@'
	    if( atPos == string::npos )
	    {
		cpLog( LOG_ERR, "Error: No @ in Speed_Dial, exiting...\n" );
		return 0;
	    }
	    
	    if( colonPos1 != colonPos2 )
	    {
		// We have a port & a password
//		cpDebug(LOG_DEBUG, "We have a port & a password" );
		
		userName = phoneNum.substr( 0, colonPos1 );
		password = phoneNum.substr( colonPos1, atPos - colonPos1 );
		destination = phoneNum.substr( atPos, colonPos2 - atPos );
		sipPort = phoneNum.substr( colonPos2 );
	    }
	    else
	    {
		// We have either a port, or a password
		if( colonPos1 < atPos )
		{
		    // We have a password, but no port
//		    cpDebug(LOG_DEBUG, "We have a password, but no port" );
		    
		    userName = phoneNum.substr( 0, colonPos1 );
		    password = phoneNum.substr( colonPos1, atPos - colonPos1 );
		    destination = phoneNum.substr( atPos );
		    sipPort = ":5060";
		}
		else
		{
		    // We have a port, but no password
//		    cpDebug(LOG_DEBUG, "We have a port, but no password" );
		    
		    userName = phoneNum.substr( 0, atPos );
		    password = "";
		    destination = phoneNum.substr( atPos, colonPos2 - atPos );
		    sipPort = phoneNum.substr( colonPos2 );
		}
	    }
	}
	// Set the SipUrl
	url = "sip:" + userName + password + destination + sipPort;

	if( atPos == string::npos )
	{
	    dialedType = dial_phone;
	}
	else
	{
	    dialedType = dial_ip;
	}
    }
    regfree(&re);

    cpDebug(LOG_DEBUG , "DigitCollector url is returning %s", url.c_str());
    return url;
}
/*
string
DigitCollector::getUrlFromIpAddr()
{
    //get the current 3 digits of the IP address.
    string url = "sip:user@";
    string firstIPpart = theSystem.getFirstIPpart();

    url += firstIPpart;   //returns the first three digits.
    //getdial returns the last IP part.
    url += ".";

    string dial2 = dial;
    dial2.erase(0, 1);

    const char *dialStr = dial2.c_str();

    if ( dialStr[0] != '0' )
        url += dial;
    else if ( dialStr[1] != '0' )
    {
        url += dialStr[1];
        url += dialStr[2];
    }
    else
        url += dialStr[2];

    if (sipPortNum == "" )
    {
        url += ":5060";
    }
    else
    {
        url += string(":");
        url += sipPortNum;

    }

    dialedType = dial_ip;

    cpDebug(LOG_DEBUG , "DigitCollector url is returning %s", url.c_str());

    return url;
}
*/
///
string

DigitCollector::getCallReturnUrl()

{

     string url = ""; 

     /*  modify  by wuaiwu for return call, 2005/4 /11 */
     assert(dial.size() == 2);
     OthsDialMapType::iterator iter_ = recallDialMap.find(dial);
     if (iter_ == recallDialMap.end())
     	{
     		url = "0000000 not found dial";
     	}
     else
     	{
     		string temp = iter_->second;
		unsigned index = atoi(temp.c_str());
		
		assert(index >= 0 && index < 10);

		if (prevInUrls.size() < index+1)
		{
			url = "0000000 no prev calls";
		}
		else
		{
			url = prevInUrls[index];
		}
     	}

	if ( url.find(dial_phone) != string::npos )
	{
            dialedType = dial_phone;
	}
        else
        {
             dialedType = dial_ip;
        }

	/* end of added */ 
     
     cpDebug(LOG_DEBUG, "------- return call Url --> %s",url.c_str());

     return url;
}

///
void
DigitCollector::addSpeedDial(const string& key, const string& phoneNum)
{
    speedDialMap[key] = phoneNum;
}

void DigitCollector::addLocalDial(const string& key, const string& phoneNum)
{
	localDialMap[key] = phoneNum;
}

/// * added by wuaiwu, 2005/4/12 */

void DigitCollector::addOthsDial(int type, const string & key, const string & index)
{
      if (type == 0)
		recallDialMap[key] = index;
	else
		specialKeyDialMap[key] = index;
}
/// /* end of added */

///
void
DigitCollector::addDialPlan(const DialingPlan* dialingPlan)
{
    if ( dialingPlan == 0 )
    {
        return;
    }

    for( DialingPlanListType::iterator iter = dialingPlanList.begin();
	 iter != dialingPlanList.end(); iter++ )
    {
        const DialingPlan* pDialingPlan = *iter;

        if( pDialingPlan != 0  && *pDialingPlan == *dialingPlan )
        {
            dialingPlanList.erase( iter );
            break;
        }
    }

    dialingPlanList.push_back( dialingPlan );

    unsigned int patternLen = dialingPlan->getDialPattern().length();

    if ( patternLen > maxDialPatternLen )
    {
        maxDialPatternLen = patternLen;
    }
}

///
void
DigitCollector::flushDialPlan()
{
    DialingPlanListType::iterator iDialingPlanList = dialingPlanList.begin();

    while ( iDialingPlanList != dialingPlanList.end() )
    {
        const DialingPlan* dialingPlan = *iDialingPlanList;
        delete dialingPlan;

        ++iDialingPlanList;
    }
    dialingPlanList.clear();
}

///
DialMethodsType
DigitCollector::findDialPlan()
{
	regex_t re;
	DialMethodsType method = INCOMPLETE_DIAL;

        /*add by Tangbiao 2005-4-4*/
        reload_AllPlan();
        /*add by Tangbiao 2005-4-4*/

	if(dialMethod == URL_DIAL)
	{
		return URL_DIAL;
	}
	
	
	
	/* added by lizhijie, for INTERNAL_IP_DIAL  */
	if(dialMethod==INTERNAL_IP_DIAL)
	{
		AssistDevice *dev = checkHardwareForLocalDial(dial);
		if(dev==0)
		{
			cpDebug(LOG_DEBUG , "device with phone number of %s is not found", dial.c_str());
			return WRONG_DIAL;
		}	
		if(dev->getHardwareStatus() != HARDWARE_AVAILABLE)
		{
			cpDebug(LOG_DEBUG , "device %d with phone %s is busy now", dev->getDeviceId(),  dial.c_str());
			return WRONG_DIAL;
		}	
	}
	/* end of added */		

	//if the number of digits already dialed is longer than max length of all
	//dialing patterns we have, then we just return WRONG_DIAL
	if ( dial.length() > maxDialPatternLen )
	{
		cpDebug(LOG_DEBUG, "WRONG_DIAL-Number of dialed digits(%d) is too long", dial.length() );
		return WRONG_DIAL;
	}

    //match the patterns.  We assume that there is only match in the list
    //of patterns we have
    
    
    
    DialingPlanListType::iterator iDialingPlanList = dialingPlanList.begin();

    cpDebug(LOG_DEBUG, "Dialed string \"%s\"", dial.c_str() );

    while ( iDialingPlanList != dialingPlanList.end() )
    {
        const DialingPlan* dialingPlan = *iDialingPlanList;

        if ( !dialingPlan )
            continue;

        const char *pattern = dialingPlan->getDialPattern().c_str();
        if ( 0 != regcomp(&re, pattern, REG_EXTENDED) )
        {
            cpDebug(LOG_DEBUG, "WRONG_DIAL-Pattern not match" );
            return WRONG_DIAL;
        }
        const char *dialstr = dial.c_str();
        if ( REG_NOMATCH == regexec( &re, dialstr, 0, NULL, 0) )
            ++iDialingPlanList;
        else
        {
            method = dialingPlan->getDialMethod();
            break;
        }
        regfree(&re);
    }

    switch ( method )
    {
        case SPEED_DIAL:
        {
            cpDebug(LOG_DEBUG, "SPEED_DIAL" );
        }
        break;

        case SPECIAL_KEY_DIAL:
        {
            cpDebug(LOG_DEBUG, "SPECIAL_KEY_DIAL" );
        }
        break;

        case NORMAL_DIAL:
        {
            cpDebug(LOG_DEBUG, "NORMAL_DIAL" );
        }
        break;

        case INTERNAL_IP_DIAL:
        {
			cpDebug(LOG_DEBUG, "INTERNAL_IP_DIAL" );
			
			/* added by lizhijie, for regexp check of device call */
			DialingPlanListType::iterator iDialingPlanList = dialingPlanList.begin();
			while ( iDialingPlanList != dialingPlanList.end() )
			{
				const DialingPlan* dialingPlan = *iDialingPlanList;
				if ( !dialingPlan )
					continue;
				if(dialingPlan->getDialMethod()==INTERNAL_IP_DIAL)
				{
					const char *pattern = dialingPlan->getDialPattern().c_str();
					if ( 0 != regcomp(&re, pattern, REG_EXTENDED) )
					{
						cpDebug(LOG_DEBUG, "WRONG_DIAL-Pattern not match" );
						method = WRONG_DIAL;
					}
					const char *dialstr = dial.c_str();
					if ( REG_NOMATCH == regexec( &re, dialstr, 0, NULL, 0) )
						method = WRONG_DIAL;
					regfree(&re);
				}
				++iDialingPlanList;
			}
			/* end of added */
        }
        break;

	 /* added by wuaiwu, 2005/4/12 */
	case CALL_RETURN_DIAL:
	{
		cpDebug(LOG_DEBUG, "CALL_RETURN_DIAL");
	}
	break;

        case INCOMPLETE_DIAL:
        {
            cpDebug(LOG_DEBUG, "INCOMPLETE_DIAL" );
        }
        break;

        default:
        {
            cpDebug(LOG_DEBUG, "WRONG_DIAL" );
        }
    }    /* switch */

    return method;
}

//
void
DigitCollector::addIncomingUrl( const string& url )
{

      if ( prevInUrls.size() >= MAX_PREV_IN_URLS_SIZE )
    {
        prevInUrls.pop_back();
    }
    prevInUrls.insert(prevInUrls.begin(), url);
}

///constructor for the dialing plan
DialingPlan::DialingPlan(string pattern, DialMethodsType method)
{
    dialPattern = pattern;
    dialMethod = method;
}

///
DialingPlan::DialingPlan(const DialingPlan& src)
{
    dialPattern = src.dialPattern;
    dialMethod = src.dialMethod;

}

///equality operator for DialingPlan.
///returns true if the dialPattern are equivalent
bool
DialingPlan::operator ==(const DialingPlan& dialPlan) const
{
    return dialPattern == dialPlan.dialPattern;
}

///
void
DialingPlan::operator =(const DialingPlan& src)
{
    dialPattern = src.dialPattern;
    dialMethod = src.dialMethod;
}


/*add by Tangbiao 2005-4-4*/	
void
DigitCollector::reload_AllPlan()
{

  int flag=0;
  UaConfiguration* configuration = UaConfiguration::instance();
  flag=configuration->getflag_change();
  if(flag==1)return; 
  flag=configuration->getflag_alter();
  if(flag==0)return;
  
  //UaConfiguration::instance()->lock_config();  
  local_config_Mutex.lock();
  flushDialPlan();
  flushSpeedDial();
  flushLocalDial();
  
   DialingPlanListType dp = configuration->getDialPlans();
   SpeedDialMapType sd = configuration->getSpeedDials();
   LocalDialMapType localDial = configuration->getLocalDials();
   local_config_Mutex.unlock();
    
   DialingPlanListType::iterator dpIter = dp.begin();
   while ( dpIter != dp.end() )
   {
        DialingPlan* dialPattern = new DialingPlan( *(*dpIter) );
        addDialPlan( dialPattern );
        dpIter++;
   }

   
   SpeedDialMapType::iterator sdIter = sd.begin();
   while ( sdIter != sd.end() )
   {
        addSpeedDial( sdIter->first, sdIter->second );
        sdIter++;
   }

   
    LocalDialMapType::iterator iter = localDial.begin();
    while ( iter != localDial.end() )
   {
	addLocalDial( iter->first, iter->second );
	iter++;
   }
   
   
   interDigitTimeout->setInitTimeout( configuration->getInitialDigitTimeout() );
   interDigitTimeout->setInterDigitTimeout( configuration->getInterDigitTimeout() );

  // UaConfiguration::instance()->unlock_config();
   //configuration->resetflag_alter();	
}

void
DigitCollector::flushSpeedDial()
{
     speedDialMap.clear();
}

void
DigitCollector::flushLocalDial()
{

   localDialMap.clear();
}



/*add by Tangbiao 2005-4-4*/	

void DigitCollector::show()
{
    cout<<"jaym_____________________test"<<endl;
    LocalDialMapType::iterator iter = localDialMap.begin();
    while ( iter != localDialMap.end() )
   {
	cout<< iter->first<<"   "<<iter->second <<endl;
	iter++;
   }

}
/*add by Tangbiao 2005-4-8*/	
#else

const unsigned int DigitCollector::MAX_PREV_IN_URLS_SIZE = 10;
bool DigitCollector::bHasLoad = false;	
LocalDialMapType DigitCollector::localDialMap;
RegularContainer DigitCollector::Regulars;
SpeedDialMapType DigitCollector::speedDialMap;
DialingPlanListType DigitCollector::dialingPlanList;
Sptr < Fifo < Sptr < SipProxyEvent > > > DigitCollector::callProcessingQueue;
OthsDialMapType 	DigitCollector::recallDialMap;
OthsDialMapType DigitCollector::specialKeyDialMap;
string DigitCollector::destHostIpAddr;
string DigitCollector::sipPortNum;
DigitTimeout* DigitCollector::interDigitTimeout;
	
DigitCollector::DigitCollector(int deviceId, Sptr < Fifo < Sptr < SipProxyEvent > > > cpq )
	:myDeviceId(deviceId)
{
	UaConfiguration* configuration = UaConfiguration::instance();
	myPending = new PendingDigitTimerEvent;
	myPending->deviceId = deviceId;
	myPending->type = INTER_DIGIT_TIME_OUT;
	myPending->eventId = 0;
	myPending->isTimeout = false;


    	dial = "";
    	dialMethod = INCOMPLETE_DIAL;
    	myDialedUrl = "";

	if(!bHasLoad)
	{

		interDigitTimeout = DigitTimeout::instance();

    		DialingPlanListType dp = configuration->getDialPlans();
    		DialingPlanListType::iterator dpIter = dp.begin();

    		string proxyServer = configuration->getProxyServer();
    		string::size_type colonPos = proxyServer.find( ":" );
    		destHostIpAddr = proxyServer.substr( 0, colonPos );
    		if ( colonPos < string::npos )
    		{
        		sipPortNum = proxyServer.substr( proxyServer.rfind( ":" ) + 1 );
    		}
    		else
    		{
        		sipPortNum = configuration->getLocalSipPort(); //configuration->getSipTransport();
    		}
	
		while ( dpIter != dp.end() )
    		{
        		DialingPlan* dialPattern = new DialingPlan( *(*dpIter) );
        		addDialPlan( dialPattern );
        		dpIter++;
    		}

		SpeedDialMapType sd = configuration->getSpeedDials();
    		SpeedDialMapType::iterator sdIter = sd.begin();
    		while ( sdIter != sd.end() )
    		{
        		addSpeedDial( sdIter->first, sdIter->second );
        		sdIter++;
    		}

    		/* local pbx call map used the method of INTERNAL_IP_CALL */
     		LocalDialMapType localDial = configuration->getLocalDials();
     		LocalDialMapType::iterator iter = localDial.begin();
     		while ( iter != localDial.end() )
     		{
	 		addLocalDial( iter->first, iter->second );
	 		iter++;
     		}
		/* end of added by lizhijie */

		/* added by wuaiwu, get return call something, 2005/4/12 */
		/* for efficiency and restrain needless temporary objects 
		 * we should get the contianer length at first and use prev-++ instead of the back-++
	 	* notices by wuaiwu, 2005/4/12 */
		for (int i = 0; i < 2; ++i)
		{
			OthsDialMapType Dials = configuration->getOthsDials(i);
			OthsDialMapType::iterator iters = Dials.begin();
			OthsDialMapType::iterator ite_end = Dials.end();
			while (iters != ite_end)
			{
				addOthsDial(i,iters->first, iters->second);
				++iters;
			}
		}

		callProcessingQueue = cpq;
	
		setRegular();

		flushDialPlan();

		bHasLoad = true;

	}	
}

void
DigitCollector::reset()
{
    dial = "";
    dialMethod = INCOMPLETE_DIAL;
    myDialedUrl = "";
    interDigitTimeout->timerReset( myPending);
}


void
DigitCollector::clear()
{
    dial = "";
    dialMethod = INCOMPLETE_DIAL;
    myDialedUrl = "";
    interDigitTimeout->timerCancel( myPending );
}


void
DigitCollector::addDigit( char digit )
{

    isPbxDial = false;	
    //cancel the previous timeout timer
    interDigitTimeout->timerCancel(myPending);
    dial += digit;

	//start the inter-digit timeout timer for the next digit
   interDigitTimeout->timerStart( myPending);

    // Check if dialing complete
    DialMethodsType stage = findDialPlan();

   if((stage == WRONG_DIAL) || (stage!=INCOMPLETE_DIAL))
   {
   	 // Notify call processing
        Sptr < UaDigitTimerEvent > timerEvent = new UaDigitTimerEvent( callProcessingQueue );
        assert( timerEvent != 0 );

	 timerEvent->callId = UaDevice::instance()->getCallId(myDeviceId);
	 timerEvent->setDeviceId( myDeviceId );
   	 callProcessingQueue->add( timerEvent );
    }

}

void DigitCollector::addPbxDigit( char digit )
{
	 isPbxDial = true;
	 
	//cancel the previous timeout timer
	interDigitTimeout->timerCancel(myPending);
	dial += digit;

	//start the inter-digit timeout timer for the next digit
	interDigitTimeout->timerStart( myPending);

    	// Check if dialing complete
    	DialMethodsType stage = findDialPlan();
	
	if((stage == WRONG_DIAL) || (stage!=INCOMPLETE_DIAL))
    	{
       	 // Notify call processing
        	Sptr < UaDigitTimerEvent > timerEvent = new UaDigitTimerEvent( callProcessingQueue );
        	assert( timerEvent != 0 );

		timerEvent->callId = UaDevice::instance()->getCallId(myDeviceId);
		timerEvent->setDeviceId( myDeviceId );
		
        	callProcessingQueue->add( timerEvent );
    	}
}

void
DigitCollector::setDialedUrl(const string& url)
{
    	dialMethod = URL_DIAL;
   	myDialedUrl = url;

    	// Notify call processing
    	Sptr < UaDigitTimerEvent > timerEvent  = new UaDigitTimerEvent( callProcessingQueue );
    	assert( timerEvent != 0 );

	timerEvent->callId = UaDevice::instance()->getCallId(myDeviceId);
	timerEvent->setDeviceId( myDeviceId );
	
    	callProcessingQueue->add( timerEvent );
}

/* added by lizhijie : check the dial method is INTERNAL_IP_DIAL is always first  */
bool DigitCollector::isValidPbxDial()
{

	bool bValid;

	dialMethod = findDialPlan();

	if(dialMethod == INTERNAL_IP_DIAL || dialMethod == PSTN_DIAL)
		bValid = true;
	else
		bValid = false;
	
	if(!bValid)
		return false;
	else
	{
		interDigitTimeout->timerCancel( myPending );
		return true;
	}

}

bool
DigitCollector::isValidDial()
{


	dialMethod = findDialPlan();

	if (INCOMPLETE_DIAL == dialMethod || WRONG_DIAL == dialMethod)
	{
		return false;
	}
	else
	{
		interDigitTimeout->timerCancel( myPending );
		return true;
	}

}


bool DigitCollector::isDialTimeout()
{
	return myPending->isTimeout;
}


string
DigitCollector::getDial()
{
    return isDialTimeout() ? dial + string("T") : dial;
}


string
DigitCollector::getUrl()
{
    string url;
    switch ( dialMethod )
    {
    case SPEED_DIAL:
	url = getUrlFromSpeedDial();
	break;
    case SPECIAL_KEY_DIAL:
	url = getUrlFromSpecKeyDial();
	break;
    case NORMAL_DIAL:
	url = getUrlFromPhoneNumDial();
	break;
	
/* uncomment this option , lizhijie 2005.02.09 */	
    case INTERNAL_IP_DIAL:
	url = getUrlFromLocalDial();//getUrlFromIpAddr();
	break;
/* end of */		

    case CALL_RETURN_DIAL:
	url = getCallReturnUrl();
	break;
    case INCOMPLETE_DIAL:
    case WRONG_DIAL:
	url = "";
	break;
    case URL_DIAL:
	url = myDialedUrl;
	break;
    case PSTN_DIAL:
	url = getPstnUrl();
	break;
    default:
	url = "";
        break;
    }

    return url;
}

DialMethodsType DigitCollector::checkDialMethod(const string& url)
{
	
    DialMethodsType method = WRONG_DIAL;

    const char *dialstr = url.c_str();
  	
    for(RegularContainer::iterator iter = Regulars.begin(); iter != Regulars.end(); iter++)
    {
    
      	if(REG_NOMATCH != regexec( &((*iter)->reg), dialstr, 0, NULL, 0))
	{
		if((*iter)->isFullMatch)
		{
			method = (*iter)->method;
			return method;
		}
	 }	
		
    }

    return method;
	
}

string DigitCollector:: getPstnUrl()
{
	string url; 
	string buf;

	if(!isPbxDial)
	{
		buf = dial;
		url = "sip:";

		if(buf[1] == '0')
		{
			if(buf[2] == '0')
				url = url+buf;
			else
				url = url+"000"+UaConfiguration::instance()->getLocalCountry()+buf.substr(2);
		}
		else
		{
			url = url+"000"+UaConfiguration::instance()->getLocalCountry()+UaConfiguration::instance()->getLocalDistrict()+\
				UaConfiguration::instance()->getLocalDevice()+buf.substr(1);
		}

		static string local = "000"+UaConfiguration::instance()->getLocalCountry()+UaConfiguration::instance()->getLocalDistrict()+UaConfiguration::instance()->getLocalDevice();

		if(url.size()>=local.size() && memcmp(local.c_str(), url.c_str(), local.size())==0)
			url = url.substr(0, url.size()-1);

		url += string("@");
		url += string("127.0.0.1");

		if (sipPortNum == "" )
		{
			url += ":5060";
		}
		else
		{
			url += string(":");
			url += UaConfiguration::instance()->getLocalSipPort ();
		}
	}	
	else
	{
		url = dial;
	}
	
	dialedType = dial_phone;

	return url;
	
}


string DigitCollector:: getUrlFromLocalDial()
{
	LocalDialMapType::iterator localDialIt = localDialMap.find(dial);

	if ( localDialIt == localDialMap.end() )
	{
		cpLog( LOG_ERR, "Local_Dial: %s  not found.", dial.c_str() );
		return "";
	}

	AssistDeviceInfo *dev = (AssistDeviceInfo *)UaDevice::instance()->getAssistDeviceInfo( localDialIt->second.c_str() );
	if(dev==0)
	{
		cpLog( LOG_ERR, "Hardware device with name of %s is not found.", localDialIt->second.c_str() );
		return "";
	}

	HardwareStatusType status = UaDevice::instance()->getHardwareStatus(dev->getDeviceId());
	
	if(status== HARDWARE_UNAVAILABLE)
	{
		cpLog( LOG_ERR, "Hardware device with name of %s is busy now.", dev->getMyDevice()->name );
		return string("");
	}
	
	string url = "sip:";
	
	url += string(dev->getPhoneNumber());
	
	url += string("@");
	url += string("127.0.0.1");
	
	if (sipPortNum == "" )
	{
		url += ":5060";
	}
	else
	{
		url += string(":");
		url += UaConfiguration::instance()->getLocalSipPort ();
	}

	cpDebug(LOG_DEBUG , "DigitCollector url is returning for Local_Dial is '%s'", url.c_str());
	dialedType = dial_phone;

	return url;
}
/* end of added */

string
DigitCollector::getUrlFromPhoneNumDial()
{
    //get the current 3 digits of the IP address.
    if(destHostIpAddr == "")
		return "";
    string url = "sip:";
    url += dial;
    url += string("@");

    url += destHostIpAddr;

    if (sipPortNum == "" )
    {
        url += ":5060";
    }
    else
    {
        url += string(":");
        url += sipPortNum;
    }

    cpDebug(LOG_DEBUG , "DigitCollector url is returning %s", url.c_str());

    dialedType = dial_phone;

    return url;
}

string
DigitCollector::getUrlFromSpecKeyDial()
{
   
     string dialStr = dial;


    string url("sip:");
    OthsDialMapType::iterator ite = specialKeyDialMap.find(dialStr);
    if (ite == specialKeyDialMap.end())
    {
    	return "";	//url = "0000000";
    }
    else 
    {
	 	url += ite->second;
    }

    url += ":5060";

    cpDebug(LOG_DEBUG , "DigitCollector url is returning %s", url.c_str());

    dialedType = dial_phone;

    return url;
}

string
DigitCollector::getUrlFromSpeedDial()
{
    regex_t re;

    //get the current 3 digits of the IP address.
    string phoneNum = "";   // String retrieved from speedDialMap
    string url = "";        // url that we return
    string userName = "";
    string password = "";
    string destination = "";
    string sipPort = "";

    SpeedDialMapType::iterator speedDialIt = speedDialMap.find(dial);
    if ( speedDialIt == speedDialMap.end() )
    {
		cpLog( LOG_ERR, "Speed_Dial: %s  not found.", dial.c_str() );
        return "";
    }
    else
    {
        phoneNum = speedDialIt->second;
    }

    if ( 0 == regcomp(&re, "[^0-9]", REG_EXTENDED) )
    {
	string::size_type colonPos1, colonPos2, atPos;

	colonPos1 = phoneNum.find( ":" );
	colonPos2 = phoneNum.rfind( ":" );
	atPos = phoneNum.find( "@" );
    
	if( colonPos1 == string::npos )
	{
	    // No ':'

	    if( atPos == string::npos )
	    {
		// No '@'
		// In this case, do we need to make sure that destination
		// is only [0-9]* ?
		userName = phoneNum;
		password = "";
		destination = "@" + destHostIpAddr;
                sipPort = ":" + sipPortNum;

	    }
	    else
	    {
		// We have an '@'
		userName = phoneNum.substr( 0, atPos );
		password = "";
		destination = phoneNum.substr( atPos );
		sipPort = ":5060";
	    }
	}
	else  // We have at least one ':'
	{
	    // In this case, we need an '@'
	    if( atPos == string::npos )
	    {
		cpLog( LOG_ERR, "Error: No @ in Speed_Dial, exiting...\n" );
		return 0;
	    }
	    
	    if( colonPos1 != colonPos2 )
	    {
		// We have a port & a password
//		cpDebug(LOG_DEBUG, "We have a port & a password" );
		
		userName = phoneNum.substr( 0, colonPos1 );
		password = phoneNum.substr( colonPos1, atPos - colonPos1 );
		destination = phoneNum.substr( atPos, colonPos2 - atPos );
		sipPort = phoneNum.substr( colonPos2 );
	    }
	    else
	    {
		// We have either a port, or a password
		if( colonPos1 < atPos )
		{
		    // We have a password, but no port
//		    cpDebug(LOG_DEBUG, "We have a password, but no port" );
		    
		    userName = phoneNum.substr( 0, colonPos1 );
		    password = phoneNum.substr( colonPos1, atPos - colonPos1 );
		    destination = phoneNum.substr( atPos );
		    sipPort = ":5060";
		}
		else
		{
		    // We have a port, but no password
//		    cpDebug(LOG_DEBUG, "We have a port, but no password" );
		    
		    userName = phoneNum.substr( 0, atPos );
		    password = "";
		    destination = phoneNum.substr( atPos, colonPos2 - atPos );
		    sipPort = phoneNum.substr( colonPos2 );
		}
	    }
	}
	// Set the SipUrl
	url = "sip:" + userName + password + destination + sipPort;

	if( atPos == string::npos )
	{
	    dialedType = dial_phone;
	}
	else
	{
	    dialedType = dial_ip;
	}
    }
    regfree(&re);

    cpDebug(LOG_DEBUG , "DigitCollector url is returning %s", url.c_str());
    return url;
}
string DigitCollector::getCallReturnUrl()
{

     string url = ""; 

     /*  modify  by wuaiwu for return call, 2005/4 /11 */
     assert(dial.size() == 2);
     OthsDialMapType::iterator iter_ = recallDialMap.find(dial);
     if (iter_ == recallDialMap.end())
     	{
     		return "";//url = "0000000 not found dial";
     	}
     else
     	{
     		string temp = iter_->second;
		unsigned index = atoi(temp.c_str());
		
		assert(index >= 0 && index < 10);

		if (prevInUrls.size() < index+1)
		{
			return "";//url = "0000000 no prev calls";
		}
		else
		{
			url = prevInUrls[index];
		}
     	}

	if ( url.find(dial_phone) != string::npos )
	{
            dialedType = dial_phone;
	}
        else
        {
             dialedType = dial_ip;
        }

	/* end of added */ 
     
     cpDebug(LOG_DEBUG, "------- return call Url --> %s",url.c_str());

     return url;
}

///
void
DigitCollector::addSpeedDial(const string& key, const string& phoneNum)
{
    speedDialMap[key] = phoneNum;
}

void DigitCollector::addLocalDial(const string& key, const string& phoneNum)
{
	localDialMap[key] = phoneNum;
}

/// * added by wuaiwu, 2005/4/12 */

void DigitCollector::addOthsDial(int type, const string & key, const string & index)
{
      if (type == 0)
		recallDialMap[key] = index;
	else
		specialKeyDialMap[key] = index;
}
/// /* end of added */

///
void
DigitCollector::addDialPlan(const DialingPlan* dialingPlan)
{
    if ( dialingPlan == 0 )
    {
        return;
    }

    for( DialingPlanListType::iterator iter = dialingPlanList.begin();
	 iter != dialingPlanList.end(); iter++ )
    {
        const DialingPlan* pDialingPlan = *iter;

        if( pDialingPlan != 0  && *pDialingPlan == *dialingPlan )
        {
            dialingPlanList.erase( iter );
            break;
        }
    }

    dialingPlanList.push_back( dialingPlan );

}

///
void
DigitCollector::flushDialPlan()
{
    DialingPlanListType::iterator iDialingPlanList = dialingPlanList.begin();

    while ( iDialingPlanList != dialingPlanList.end() )
    {
        const DialingPlan* dialingPlan = *iDialingPlanList;
        delete dialingPlan;

        ++iDialingPlanList;
    }
    dialingPlanList.clear();
}

void DigitCollector::setRegular()
{

    bool bExist;
    int iOrigLen;
    int iNowLen;
    regex_t re;
    Sptr<Regular>	regular;

	   	
    DialingPlanListType::iterator iDialingPlanList = dialingPlanList.begin();

    while ( iDialingPlanList != dialingPlanList.end() )
    {
    	 const DialingPlan* dialingPlan = *iDialingPlanList;

        if ( !dialingPlan )
            continue;

        char *pattern = (char *)dialingPlan->getDialPattern().c_str();
	 iOrigLen = strlen(pattern);
	 iNowLen = iOrigLen;
	 
	while(iNowLen)
	{
		bExist = false;
		if ( 0 != regcomp(&re, pattern, REG_EXTENDED) )
        	{
        		//the full sting is not a corret regular expression, so ignore it
           		if(iNowLen == iOrigLen)
				break;
        	}
		else
		{
			//check if it is an exist regular, if so, ignore it 	
		    	for(RegularContainer::iterator iter=Regulars.begin(); iter!=Regulars.end();  iter++)
    		   	{
        	   		Sptr<Regular> regular= (*iter);
				regex_t reg = regular->reg;	
		       	if(!memcmp(&reg, &re, sizeof(regex_t)))
		       	{
					regfree(&re);
					bExist = true;
					break;
		       	}	
		    	}

			if(bExist)
				continue;
				
			regular = new Regular;
			regular->reg = re;
			regular->method = dialingPlan->getDialMethod();
			if(iNowLen == iOrigLen)
				regular->isFullMatch = true;
			else
				regular->isFullMatch = false;
			Regulars.push_back(regular);
			
		}
      		pattern[iNowLen-1]=0;
		iNowLen--;
	}
	iDialingPlanList++;
    }

}

///
DialMethodsType DigitCollector::findDialPlan()
{

   bool bValidPattern;
   //0 no math, 1 has part match, 2 full match
   int iFlag = 0; 
	
    DialMethodsType method = WRONG_DIAL;

    const char *dialstr = dial.c_str();

	
    for(RegularContainer::iterator iter = Regulars.begin(); iter != Regulars.end(); iter++)
    {
    
       bValidPattern = false;
    	if(isPbxDial)
    	{
    		if((*iter)->method == INTERNAL_IP_DIAL || (*iter)->method == PSTN_DIAL)
    		{
    			bValidPattern = true;
    		}

    	}
	else
		bValidPattern = true;
	
	if(bValidPattern)
	{
		if(REG_NOMATCH != regexec( &((*iter)->reg), dialstr, 0, NULL, 0))
		{
			if((*iter)->isFullMatch)
			{
				iFlag = 2;
				method = (*iter)->method;
				break;
			}
			iFlag = 1;
		}
	}	
    }

    switch(iFlag)
    {
    	case 1:
			if(dial.length()<=30)
				method = INCOMPLETE_DIAL;
			break;
	case 0:
	case 2:
	default:
			break;
    }

    switch ( method )
    {
        case SPEED_DIAL:
        {
            cpDebug(LOG_DEBUG, "SPEED_DIAL" );
        }
        break;

        case SPECIAL_KEY_DIAL:
        {
            cpDebug(LOG_DEBUG, "SPECIAL_KEY_DIAL" );
        }
        break;

        case NORMAL_DIAL:
        {
            cpDebug(LOG_DEBUG, "NORMAL_DIAL" );
        }
        break;

        case INTERNAL_IP_DIAL:
        {
			cpDebug(LOG_DEBUG, "INTERNAL_IP_DIAL" );
        }
        break;
		
	/* added by wuaiwu, 2005/4/12 */
	case CALL_RETURN_DIAL:
	{
		cpDebug(LOG_DEBUG, "CALL_RETURN_DIAL");
	}
	break;

        case INCOMPLETE_DIAL:
        {
            cpDebug(LOG_DEBUG, "INCOMPLETE_DIAL" );
        }
        break;

	case PSTN_DIAL:
	{
		cpDebug(LOG_DEBUG, "PSTN_DIAL");
	}
	break;
        default:
        {
            cpDebug(LOG_DEBUG, "WRONG_DIAL" );
        }
    }    /* switch */

    return method;
}

//
void DigitCollector::addIncomingUrl( const string& url )
{

    if ( prevInUrls.size() >= MAX_PREV_IN_URLS_SIZE )
    {
        prevInUrls.pop_back();
    }
    prevInUrls.insert(prevInUrls.begin(), url);
}

///constructor for the dialing plan
DialingPlan::DialingPlan(string pattern, DialMethodsType method)
{
    dialPattern = pattern;
    dialMethod = method;
}

///
DialingPlan::DialingPlan(const DialingPlan& src)
{
    dialPattern = src.dialPattern;
    dialMethod = src.dialMethod;
}

///equality operator for DialingPlan.
///returns true if the dialPattern are equivalent
bool DialingPlan::operator ==(const DialingPlan& dialPlan) const
{
    return dialPattern == dialPlan.dialPattern;
}

///
void DialingPlan::operator =(const DialingPlan& src)
{
    dialPattern = src.dialPattern;
    dialMethod = src.dialMethod;
}


#endif

#endif
