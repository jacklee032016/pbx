/*
* $Id: DialingValidator.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"
#include <cassert>
#include <string.h>

#include <Data.hxx>
#include <cpLog.h>
#include "DigitEvent.hxx"
#include "DigitTimeoutEvent.hxx"
#include "PbxAgent.hxx"
#include "EndPoint.hxx"

#include "DeviceMgr.hxx"

#include "DialingValidator.hxx"
using namespace Assist;

const unsigned int DialingValidator::MAX_PREV_IN_URLS_SIZE = 10;

DialingValidator::DialingValidator()
{
	maxDialPatternLen = 20;
	/*dial = "";
	isNewDigit = false;
	dialMethod = INCOMPLETE_DIAL;
	myDialedUrl = "";
	*/

	init();
}

void DialingValidator::init()
{
	PbxConfiguration *configuration = PbxConfiguration::instance();


#if 0
	DialingPlanListType dp = configuration->getDialPlans();
	DialingPlanListType::iterator dpIter = dp.begin();
	while ( dpIter != dp.end() )
	{
		DialingPlan* dialPattern = new DialingPlan( *(*dpIter) );
		addDialPlan( dialPattern );
		dpIter++;
	}
#endif 

	dialingPlanList = configuration->getDialPlans();

	SpeedDialMapType sd = configuration->getSpeedDials();
	SpeedDialMapType::iterator sdIter = sd.begin();
	while ( sdIter != sd.end() )
	{
		addSpeedDial( sdIter->first, sdIter->second );
		sdIter++;
	}

	LocalDialMapType localDial = configuration->getLocalDials();
	LocalDialMapType::iterator iter = localDial.begin();
	while ( iter != localDial.end() )
	{
		addLocalDial( iter->first, iter->second );
		iter++;
	}

	initRegular();
}


void DialingValidator::initRegular()
{
	bool	bExist;
	int 	iOrigLen;
	int 	iNowLen;
	regex_t re;
	Sptr<Regular>	regular;

	PbxConfiguration* pconf = PbxConfiguration::instance();//->getDialPlans();
	DialingPlanListType::iterator iDialingPlanList = pconf->getDialPlans().begin();//dialingPlanList.begin();

	while ( iDialingPlanList !=  pconf->getDialPlans().end() )
	{
		const DialingPlan* dialingPlan = *iDialingPlanList;

		if ( !dialingPlan )
			continue;

		char *pattern =  const_cast<char*>( (*iDialingPlanList)->getDialPattern().c_str());// (char *)dialingPlan->getDialPattern().c_str();
		cpLog(LOG_DEBUG, "Dialing Pattern: %s ---- %s", pattern, (*iDialingPlanList)->getDialPattern().c_str());
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
			{//check if it is an exist regular, if so, ignore it 	
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
				regular->value = pattern;
				regular->method = (*iDialingPlanList)->getDialMethod(); // dialingPlan->getDialMethod();
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

#if 0//WITH_DEBUG
	for(RegularContainer::iterator iter = Regulars.begin(); iter != Regulars.end(); iter++)
	{
		Sptr<Regular> regular= (*iter);
		cpLog(LOG_DEBUG, "Reguler : %s \tType : %d \tMatch : %s", regular->value.c_str(), regular->method, regular->isFullMatch==true?"Full":"Part");
	}
#endif
}

void DialingValidator::voipInit()
{
#if 0
	//Get digit plan from ua.cfg
	PbxConfiguration* conf = PbxConfiguration::instance();
	
	localDialMap = conf->getLocalDials();
	speedDialMap = conf->getSpeedDials();
	dialingPlanList = conf->getDialPlans();
	recallDialMap = conf->getOthsDials(0);
	specialKeyDialMap = conf->getOthsDials(1);

    	string proxyServer = conf->getProxyServer();
    	string::size_type colonPos = proxyServer.find( ":" );
    	destHostIpAddr = proxyServer.substr( 0, colonPos );
    	if ( colonPos < string::npos )
    	{
        	sipPortNum = proxyServer.substr( proxyServer.rfind( ":" ) + 1 );
    	}
    	else
    	{
        	sipPortNum = conf->getLocalSipPort(); //configuration->getSipTransport();
    	}
	if(sipPortNum == "")
		sipPortNum = "5060";
#endif		
	//flushDialPlan();	
}


//dialing_state_t DialingValidator::getDialedState(const string& dialing)
dialing_state_t DialingValidator::addDigit( Sptr <DigitEvent> digit)
{
	Sptr <EndPoint> ep = digit->getEndPoint();
	assert(ep != 0);

#if 0
	ep->setDialing(digit->myDigit);
#endif

	// Check if dialing complete
	DialMethodsType stage = INCOMPLETE_DIAL;

	cpLog(LOG_DEBUG, "-----------------------EP get Digits: '%s'\n", ep->getDialing().c_str());
	stage = getDialedMethod(ep->getDialing()); 

	if ( stage == WRONG_DIAL ) 
		return dialing_error;
	else if ( stage == INCOMPLETE_DIAL)
		return dialing_not_finished;
	else 
	{
		cpLog(LOG_DEBUG, "COMPLETE_DIAL and OK");
		return dialing_finished;
	}
}

dialing_state_t DialingValidator::addDigit( const string& dial)
{
	DialMethodsType stage = INCOMPLETE_DIAL;

	stage = getDialedMethod(dial); 

	if ( stage == WRONG_DIAL ) 
	{
		cpLog(LOG_DEBUG,"WRONG_DIAL");
		return dialing_error;
	}
	else if ( stage == INCOMPLETE_DIAL)
	{
		cpLog(LOG_DEBUG,"INCOMPLETE_DIAL");
		return dialing_not_finished;
	}
	else 
	{
		cpLog(LOG_DEBUG,"stage = %d", stage);
		return dialing_finished;   //*************
	}
}

DialMethodsType DialingValidator::getDialedMethod(const string& dialing)
{
	//dial = dialing;
	const char *extCode = PbxConfiguration::instance()->getExtAccessCode();
	const char *dialingstr = dialing.c_str();
	int extLength = strlen(extCode);
	int dialingLength = strlen(dialingstr );
	int length = ( extLength> dialingLength) ? dialingLength:extLength ;

	/* check extension phone number */
	if( strncmp(extCode, dialingstr, length) == 0)
	{
		cpDebug(LOG_DEBUG, "Internal extension dialing");
		if(extLength > dialingLength ||(extLength +PbxConfiguration::instance()->getExtLength() )>dialingLength )
			return INCOMPLETE_DIAL;
		else if(dialingLength > (extLength +PbxConfiguration::instance()->getExtLength() ) )
			return WRONG_DIAL;
		else if(dialingLength == (extLength + PbxConfiguration::instance()->getExtLength() ) )
		{
			cpDebug(LOG_DEBUG, "Internal Extension dialing : complete");
			return EXTENSION_DIAL;
		}	
	}
	
#if 1	
	return checkDialMethod(dialing);
#else
	if (strcmp(epName, "VOIP-EP") == 0 )
		return findDialPlan(dialing);
	else 
		return checkDialMethod(dialing);
#endif	
}

/* added by lizhijie : check the dial method is INTERNAL_IP_DIAL is always first  */
bool DialingValidator::isValidPbxDial(const string& dialing)
{
	//dialMethod = findDialPlan();
	DialMethodsType 		dialMethod = INCOMPLETE_DIAL;

	dialMethod = getDialedMethod(dialing);

	if (INCOMPLETE_DIAL == dialMethod || WRONG_DIAL == dialMethod)
	{
		return false;
	}
	else if(INTERNAL_IP_DIAL != dialMethod )
		return false;
	else
	{
		return true;
	}
}


bool DialingValidator::isValidDial(const string& dialing)
{

	//dialMethod = findDialPlan();
	//isNewDigit = false;
	DialMethodsType 		dialMethod = INCOMPLETE_DIAL;

	dialMethod = getDialedMethod(dialing);

	if (INCOMPLETE_DIAL == dialMethod || WRONG_DIAL == dialMethod)
	{
		return false;
	}
	else
	{
		return true;
	}

}

string DialingValidator::getPhoneNumber( int deviceId )
{
	return PbxConfiguration::instance()->getUserName();
}

void DialingValidator::addSpeedDial(const string& key, const string& phoneNum)
{
	speedDialMap[key] = phoneNum;
}

void DialingValidator::addLocalDial(const string& key, const string& phoneNum)
{
	localDialMap[key] = phoneNum;
}


void DialingValidator::addDialPlan(const DialingPlan* dialingPlan)
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


void DialingValidator::flushDialPlan()
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


DialMethodsType DialingValidator::findDialPlan(const string& dial)
{
	regex_t re;
	DialMethodsType method = INCOMPLETE_DIAL;
	cpLog(LOG_DEBUG, "======>>>>>findDialPlan:%s", dial.c_str());


#if 0
	//Del at 06.01.18	

	if(dialMethod == URL_DIAL)
	{
		return URL_DIAL;
	}
#endif

#if 0	
	/* added by lizhijie, for INTERNAL_IP_DIAL  */
	if(dialMethod==INTERNAL_IP_DIAL)
	{
		AssistDevice *dev = checkHardwareForLocalDial(dial);
		if(dev==0)
		{
			cpLog( LOG_DEBUG , "device with phone number of %s is not found", dial.c_str());
			return WRONG_DIAL;
		}	
		if(dev->getHardwareStatus() != HARDWARE_AVAILABLE)
		{
			cpLog( LOG_DEBUG , "device %d with phone %s is busy now", dev->getDeviceId(),  dial.c_str());
			return WRONG_DIAL;
		}	
	}
	/* end of added */		
#endif

	//if the number of digits already dialed is longer than max length of all
	//dialing patterns we have, then we just return WRONG_DIAL
	if ( dial.length() > maxDialPatternLen )
	{
		cpLog( LOG_DEBUG, "WRONG_DIAL-Number of dialed digits(%d) is too long (%d)", dial.length(),  maxDialPatternLen);
		return WRONG_DIAL;
	}

	DialingPlanListType::iterator iDialingPlanList = dialingPlanList.begin();
	cpLog( LOG_DEBUG, "Dialed string \"%s\"", dial.c_str() );

	while ( iDialingPlanList != dialingPlanList.end() )
	{
		const DialingPlan* dialingPlan = *iDialingPlanList;
		if ( !dialingPlan )
			continue;

		const char *pattern = dialingPlan->getDialPattern().c_str();
		if ( 0 != regcomp(&re, pattern, REG_EXTENDED) )
		{
			cpLog( LOG_DEBUG, "WRONG_DIAL-Pattern not match" );
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
			cpLog( LOG_DEBUG, "SPEED_DIAL" );
			break;
		}

		case INTERNAL_DIAL:
		{
			cpLog( LOG_DEBUG, "INTERNAL_DIAL" );
			break;
		}

		case NORMAL_DIAL:
		{
			cpLog( LOG_DEBUG, "NORMAL_DIAL" );
			break;
		}

		case INTERNAL_IP_DIAL:
		{
			cpLog( LOG_DEBUG, "INTERNAL_IP_DIAL" );
			
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
						cpLog( LOG_DEBUG, "WRONG_DIAL-Pattern not match" );
						method = WRONG_DIAL;
					}
					const char *dialstr = dial.c_str();
					if ( REG_NOMATCH == regexec( &re, dialstr, 0, NULL, 0) )
						method = WRONG_DIAL;
					regfree(&re);
				}
				++iDialingPlanList;
			}
			break;
			/* end of added */
		}

		case CALL_RETURN_DIAL:
		{
			cpLog(LOG_DEBUG, "CALL_RETURN_DIAL");
			break;
		}

		case INCOMPLETE_DIAL:
		{
			cpLog( LOG_DEBUG, "INCOMPLETE_DIAL" );
			break;
		}

		default:
		{
			cpLog( LOG_DEBUG, "WRONG_DIAL" );
		}
	}    /* switch */

	return method;
}

/* only chech external phone number in this function */
DialMethodsType DialingValidator::checkDialMethod(const string& url)
{
	cpLog(LOG_DEBUG,"=====dial=%s\n", url.c_str());

	DialMethodsType method = PbxAgent::instance()->getGateway()->checkAcceddCodeDialMethod(url);
	if(method != CONTINUE_CHECK_DIAL )
		return method;

#if 0	
	int length = 0;
	const char *consoleCode = PbxAgent::instance()->getGateway()->getConsoleAccessCode();
	int accessLength = PbxConfiguration::instance()->getAccessLength();
	length = ((int)strlen(dialstr) >accessLength ) ? accessLength:strlen(dialstr);
	
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
	const char *dialstr = url.c_str();

	//if the number of digits already dialed is longer than max length of all
	//dialing patterns we have, then we just return WRONG_DIAL
	if ( url.length() > maxDialPatternLen )
	{
		cpLog( LOG_DEBUG, "WRONG_DIAL-Number of dialed digits(%d) is too long (%d)", url.length(),  maxDialPatternLen);
		return WRONG_DIAL;
	}

	for(RegularContainer::iterator iter = Regulars.begin(); iter != Regulars.end(); iter++)
	{
		if(REG_NOMATCH != regexec( &((*iter)->reg), dialstr, 0, NULL, 0))
		{
			Sptr<Regular> regular= (*iter);
			if((*iter)->isFullMatch)
			{
				method = (*iter)->method;
				cpDebug(LOG_DEBUG, "Complete Match Dial : reg : %s, \tDialing : %s, \tMethod : %d", regular->value.c_str(), dialstr, method);
				return method;
			}
			else
			{
				cpDebug(LOG_DEBUG, "Partial Match Dial : reg : %s, \tDialing : %s", regular->value.c_str(), dialstr);
				method = INCOMPLETE_DIAL;
			}
		}
	}

	cpLog(LOG_DEBUG, "The method = %d)))))--", method);

	return method;
}


void DialingValidator::addIncomingUrl( const string& url )
{
	if ( prevInUrls.size() >= MAX_PREV_IN_URLS_SIZE )
	{
		prevInUrls.pop_back();
	}
	prevInUrls.insert(prevInUrls.begin(), url);
}


void DialingValidator::flushSpeedDial()
{
	speedDialMap.clear();
}


void DialingValidator::flushLocalDial()
{
	localDialMap.clear();
}

DialMethodsType DialingValidator::findVoipDialPlan(const string& dial)
{

//0 no math, 1 has part match, 2 full match
	int iFlag = 0; 
   	regmatch_t pmatch[3];
    	DialMethodsType method = WRONG_DIAL;
    	const char *dialstr = dial.c_str();
		
	int timr=strlen(dialstr) -1; //shreshold of part dial	
	assert(timr >= 0);
	int tmp = timr;	

	//Added by chensir
	string myDialedUrl = "";

    	for(RegularContainer::iterator iter = Regulars.begin(); iter != Regulars.end(); iter++)
    	{
    		//if(isPbxDial && ((*iter)->method != INTERNAL_IP_DIAL && (*iter)->method != PSTN_DIAL))
    		if(((*iter)->method != INTERNAL_IP_DIAL && (*iter)->method != PSTN_DIAL))
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

    	if(tmp > timr)iFlag = 1;
	
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
#if 0				
					case TOWNSMAN:
						myDialedUrl = PbxConfiguration::instance()->getLocalDistrict() +myDialedUrl;
						myDialedUrl = PbxConfiguration::instance()->getLocalCountry() + myDialedUrl;
						//cpDebug(LOG_DEBUG, "REAL: %s", dial.c_str());
						//printf("_________townsman: %s : %d\n", myDialedUrl.c_str(), TOWNSMAN);
						break;

					case HOMELAND_USER1:
					case HOMELAND_USER2:
					case HOMELAND_USER3:
					case HOMELAND_USER4:
						myDialedUrl = PbxConfiguration::instance()->getLocalCountry() + myDialedUrl;
						//printf("_________homelanduser: %s : %d/%d\n", myDialedUrl.c_str(), HOMELAND_USER1,HOMELAND_USER2);
						//cpDebug(LOG_DEBUG, "REAL: %s", dial.c_str());
						break;
#endif
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

		case INTERNAL_DIAL:        
		{
			cpDebug(LOG_DEBUG, "INTERNAL_DIAL" );           
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

	//Del at 06.01.18
	//dialMethod = method;

    return method;
}

//const string& DialingValidator::getUrl()
string DialingValidator::getUrl(const string& dstr)
{

	//if (strcmp(epName, "VOIP-EP") != 0)
		//return dial;
		
	//if (dialMethod == URL_DIAL) 
		//return myDialedUrl;
    	//switch ( dialMethod )

	string 			myDialedUrl = "";
	DialMethodsType 	dialMethod = INCOMPLETE_DIAL;

	dialMethod = checkDialMethod(dstr);
	
    	switch (dialMethod)
    	{
    		case SPEED_DIAL:
			myDialedUrl = getUrlFromSpeedDial(dstr);
			break;
	
    		case INTERNAL_DIAL:
			myDialedUrl = getUrlFromSpecKeyDial(dstr);/*not used, lizhijie, 2006.05.11 */
			break;
		
    		case NORMAL_DIAL:
			myDialedUrl = getUrlFromPhoneNumDial(dstr);
			break;
	
	/* uncomment this option , lizhijie 2005.02.09 */	
    		case INTERNAL_IP_DIAL:
			myDialedUrl = getUrlFromLocalDial(dstr);//getUrlFromIpAddr();
			break;
	/* end of */		

    		case CALL_RETURN_DIAL:
			myDialedUrl = getCallReturnUrl(dstr);
			break;

    		case DIRECT_DIAL:
			myDialedUrl = getUrlFromDirectDial();
			cpDebug(LOG_DEBUG, "MYDIALEDURL: %s", myDialedUrl.c_str());
			break;
		
    		case INCOMPLETE_DIAL:
    		case WRONG_DIAL:
    		case URL_DIAL:
			break;
    		default:
			break;
    	}
	
	return myDialedUrl;
}



string  DialingValidator:: getUrlFromLocalDial(const string& dial)
{
	//myDialedUrl = "";
	string  myDialedUrl = "";
	
	LocalDialMapType::iterator localDialIt = localDialMap.find(dial);
	if ( localDialIt == localDialMap.end() )
	{
		cpLog( LOG_ERR, "Local_Dial: %s  not found.", dial.c_str() );
		return myDialedUrl;
	}

	myDialedUrl = "sip:";
	
	//myDialedUrl += string(dev->getPhoneNumber());
	
	myDialedUrl += string("@");
	myDialedUrl += string("127.0.0.1:")+sipPortNum;
	
	cpDebug(LOG_DEBUG , "DialingValidator url is returning for Local_Dial is '%s'", myDialedUrl.c_str());
	dialedType = dial_phone;

	//Added 
	return myDialedUrl ;
	
}


string  DialingValidator::getUrlFromPhoneNumDial(const string& dial)
{

	string  myDialedUrl = "";
	
    //get the current 3 digits of the IP address.
	if(destHostIpAddr == "")
 	{
 		//myDialedUrl = "";
	  	return myDialedUrl; 
    	}
    	myDialedUrl = "sip:";
    	myDialedUrl += dial;
    	myDialedUrl += string("@");
    	myDialedUrl += destHostIpAddr+":"+sipPortNum;

    	cpDebug(LOG_DEBUG , "DialingValidator url is returning %s", myDialedUrl.c_str());
    	dialedType = dial_phone;

	return myDialedUrl;
	
}


string DialingValidator::getUrlFromSpecKeyDial(const string& dial)
{
	string dialStr = dial;
    	string myDialedUrl = "sip:";
		
    	OthsDialMapType::iterator ite = specialKeyDialMap.find(dialStr);
    	if (ite == specialKeyDialMap.end())
    	{
    		myDialedUrl = "";
    		return myDialedUrl;	//url = "0000000";
    	}
    	else 
    	{
	 	myDialedUrl += ite->second;
    	}

    	myDialedUrl += ":" + sipPortNum; // ":5060";
 	cpDebug(LOG_DEBUG , "DialingValidator url is returning %s", myDialedUrl.c_str());
	dialedType = dial_phone;
	return myDialedUrl;
	
}



string DialingValidator::getUrlFromSpeedDial(const string& dial)
{
	regex_t re;

    	//get the current 3 digits of the IP address.
    	string phoneNum = "";   // String retrieved from speedDialMap
    	string userName = "";
    	string password = "";
    	string destination = "";
    	string sipPort = "";
    	//myDialedUrl = "";
    	string  myDialedUrl = "";
	
    	SpeedDialMapType::iterator speedDialIt = speedDialMap.find(dial);
    	if ( speedDialIt == speedDialMap.end() )
    	{ 
		cpLog( LOG_ERR, "Speed_Dial: %s  not found.", dial.c_str() );
		return myDialedUrl;
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
			// No '@', In this case, do we need to make sure that 
			//destination, is only [0-9]* ?
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
				sipPort = ":"+sipPortNum; // ":5060";
	    		}
		}
		else  // We have at least one ':'
		{
	    		// In this case, we need an '@'
	    		if( atPos == string::npos )
	    		{
				cpLog( LOG_ERR, "Error: No @ in Speed_Dial, exiting...\n" );
				return myDialedUrl;
	    		} 
	    		if( colonPos1 != colonPos2 )
	    		{
				// We have a port & a password
				//cpDebug(LOG_DEBUG, "We have a port & a password" );
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

    	cpDebug(LOG_DEBUG , "DialingValidator url is returning %s", myDialedUrl.c_str());

	return myDialedUrl;
	
}



string DialingValidator::getCallReturnUrl(const string& dial)
{
     string  myDialedUrl = "";

     /*  modify  by wuaiwu for return call, 2005/4 /11 */ 
     OthsDialMapType::iterator iter_ = recallDialMap.find(dial);
     if (iter_ == recallDialMap.end())
     	{
     		//myDialedUrl = "";
     		return myDialedUrl;//url = "0000000 not found dial";
     	}
     else
     	{
     		string temp = iter_->second;
		unsigned index = atoi(temp.c_str());
		
		//assert(index >= 0 && index < 10);

		if (prevInUrls.size() < index+1)
		{
			myDialedUrl = "";
			return myDialedUrl;//url = "0000000 no prev calls";
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

	return myDialedUrl;

}



string DialingValidator::getUrlFromDirectDial()
{
	//Added 
	string  myDialedUrl = "";
#if 0	
	string proxyServer = PbxConfiguration::instance()->getProxyServer();
	if (proxyServer == "")
	{
		//myDialedUrl = "";
		return myDialedUrl;
	}

	string::size_type pos = 0; //;= myDialedUrl.find("#");
	string filter("#*");
	while ((pos = myDialedUrl.find_first_of(filter, pos)) != string::npos)
		myDialedUrl.erase(pos,1);
	static string local =  PbxConfiguration::instance()->getLocalCountry() +
 	 		           PbxConfiguration::instance()->getLocalDistrict() +
 	 		           PbxConfiguration::instance()->getLocalDevice();

	if(myDialedUrl.size()>local.size() && memcmp(myDialedUrl.c_str(), local.c_str(), local.size())==0)
		myDialedUrl = "sip:"+myDialedUrl+"@127.0.0.1:"+sipPortNum;
	else	
	{
		myDialedUrl = "sip:" + myDialedUrl + "@" + proxyServer;
		pos = proxyServer.find(":");
		if (pos == string::npos)
		{
			myDialedUrl += ":"+sipPortNum;
		}
	}	
#endif

	return myDialedUrl;
}


string DialingValidator:: getDialedType()
{
	return dialedType;
};

