
#ifndef __DIALING_VALIDATOR_HXX__
#define __DIALING_VALIDATOR_HXX__
/*
* $Id: DialingValidator.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include <locale.h>
#include <sys/types.h>
#include <regex.h>
#include <string>
#include <deque>
#include <map>

#include <Sptr.hxx>
#include <PbxEvent.hxx>

#include "vpbx_globals.h"
#include "DigitEvent.hxx"
#include "PbxConfiguration.hxx"

namespace Assist
{

///For voip 
#define   HOMELAND_USER1        9+1
#define   HOMELAND_USER2        10+1
#define   HOMELAND_USER3        11+1
#define   HOMELAND_USER4        12+1
#define   TOWNSMAN			    8+1

const string dial_ip = "IP";
const string dial_phone = "phone";

typedef struct
{
	bool 				isFullMatch;
	DialMethodsType 		method;
	
	string 				value;
	regex_t 				reg;
}Regular;

typedef list< Sptr<Regular> > 		RegularContainer;
typedef map<string, string> 		OthsDialMapType;
/////////

//class DigitEvent;

typedef enum
{
	dialing_not_finished = 0,
	dialing_finished,
	dialing_error,
	dialing_unknwon
}dialing_state_t;

/* only dialing rule validate, no timer out operation is need */
class DialingValidator
{
	public:
		DialingValidator();

		//void reset(); 
		//void clear();

		bool isValidDial(const string& dialing);

		bool isValidPbxDial(const string& dialing);

		//string getDial();

		void addDialPlan(const DialingPlan* dialingPlan);
		void addSpeedDial(const string& key, const string& phoneNum);

		void addLocalDial(const string& key, const string& phoneNum);

		void addIncomingUrl( const string& url );		
		//void setDialedUrl(const string& url);
	
		string getPhoneNumber( int deviceId );
		
		void flushDialPlan();
		void flushSpeedDial();
		void flushLocalDial();

		//const string& getUrl();
		//const string getUrl(const string&dstr, DialMethodsType dtype);
		string getUrl(const string& dstr);
		DialMethodsType checkDialMethod(const string& url);
		string  getUrlFromLocalDial(const string& dial);
		string getUrlFromPhoneNumDial(const string& dial);
		string getUrlFromSpecKeyDial(const string& dial);
		string getUrlFromSpeedDial(const string& dial);
		string getCallReturnUrl(const string& dial);
		string getUrlFromDirectDial();
		string getDialedType();
		DialMethodsType getDialedMethod(const string& dialing);
		//dialing_state_t getDialedState(const string& dialing);
		dialing_state_t  addDigit( Sptr <DigitEvent> digit);
		dialing_state_t addDigit( const string& dial);

     	
	protected:

	private:
		void initRegular();

		DialMethodsType findDialPlan(const string& dialing);
		DialMethodsType findVoipDialPlan(const string& dialing);
		void voipInit();
		void init();


		/* dial msg currently */
		/* holding the dial digits */
		/*
		string 							dial;
		bool 							isNewDigit;
		DialMethodsType 					dialMethod;
		*/
		unsigned int 						maxDialPatternLen;
		string 							dialedType;
		LocalDialMapType 					localDialMap;
		SpeedDialMapType 				speedDialMap;
		DialingPlanListType 				dialingPlanList;
		//Added for VOIP
		OthsDialMapType					recallDialMap;
		OthsDialMapType					specialKeyDialMap;	
		//string 							myDialedUrl;

        	//stores the url of the last few incoming calls
        	/* modify by wuaiwu, 2005/4/12. deque support random asscessing but list can't */
        	deque < string > 					prevInUrls;
		static const unsigned int 			MAX_PREV_IN_URLS_SIZE;
		
		//For PSTN, timer.
	#if 0
		Sptr <PendingDigitTimerEvent> 		myPending;
		Sptr <EndPoint> 					myEp;
		PbxTimeoutThread* 				interPbxTimeoutThread;
	#endif

		//For voip
		string 							destHostIpAddr;
       	string 							sipPortNum;
		RegularContainer 					Regulars;
		
};

}

#endif

