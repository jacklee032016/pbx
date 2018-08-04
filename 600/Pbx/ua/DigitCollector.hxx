#ifndef DigitCollector_HXX
#define DigitCollector_HXX

/*
 * $Id: DigitCollector.hxx,v 1.6 2007/05/13 20:49:37 lizhijie Exp $
 */

/*
	modify this class by wuaiwu, 2005/08/04

	1. restrained insignificant copy
	Something (just some variables, such as dialplan, speedDials, localDials ...) 
	all devices (maping to digitcollectors) need only one. And these could be obtianed from Uaconfiguration
	and shoud be attained from it. 
	Modify for that as follows:
	static SpeedDialMapType&  replace -->>  static SpeedDialMapType
	SpeedDialMapType& DigitCollector::speedDialMap = UaConfiguration::instance()->getSpeedDials();
	...
	The correlative update of this in 
	Ua.cxx 
	Uaconfiguation.hxx/Uaconfiguration.cxx.
	Please reference these files.

	
*/

#include <locale.h>
#include <sys/types.h>
#ifdef _WIN32
#define __STDC__ 1
extern "C" {
#include <regex.h>
}
#else
#include <regex.h>
#endif
#include <string>
#include <deque>
#include <map>

#include <Sptr.hxx>
#include <SipProxyEvent.hxx>

#include "DigitTimeout.hxx"

namespace Vocal
{

//#define	INTERNATIONAL_USER1 15
//#define   INTERNATIONAL_USER2 16
#define   HOMELAND_USER1        9+1
#define   HOMELAND_USER2        10+1
#define   HOMELAND_USER3        11+1
#define   HOMELAND_USER4        12+1
#define   TOWNSMAN			    8+1


class AssistDevice;

const string dial_ip = "IP";
const string dial_phone = "phone";

typedef map < string, string > SpeedDialMapType;

/* map of device name and INTERNAL_IP_DIAL phone number 
added by lizhijie, 2005.02.14 */
typedef map < string, string > LocalDialMapType;
typedef LocalDialMapType PstnDialMapType;
/* end of added */

/* added by wuaiwu, 2005/4/12. map for return call */
typedef map<string, string> OthsDialMapType;
typedef map<string, string> TransferMapType;
typedef map<string, string>::iterator TITER;
/* end of added */

typedef	map <int, bool> 	PstnHwStatusMap;

typedef enum
{
	WRONG_DIAL = -1,
	SPEED_DIAL,			/* 0 */
	SPECIAL_KEY_DIAL,		/* 1 */
	NORMAL_DIAL,			/* 2 */
	INTERNAL_IP_DIAL,		/* 3 */
	CALL_RETURN_DIAL,		/* 4 */
	URL_DIAL,				/* 5 */
	PSTN_DIAL,				/* 6 */
	DIRECT_DIAL,			/* 7 */
	INCOMPLETE_DIAL		/* 8 */
} DialMethodsType;

typedef struct
{
	bool isFullMatch;
	DialMethodsType method;
	string value;
	regex_t reg;
}Regular;

typedef list< Sptr<Regular> > RegularContainer;

class DialingPlan
{
    public:
        DialingPlan(string pattern, DialMethodsType method);

        DialingPlan(const DialingPlan& src);

        bool operator ==(const DialingPlan& dialPlan ) const;
        void operator =(const DialingPlan& src);
        DialMethodsType getDialMethod() const
        {
            return dialMethod;
        };
        const string& getDialPattern() const
        {
            return dialPattern;
        };
    private:
        string dialPattern;
        DialMethodsType dialMethod;
};

typedef list < const DialingPlan* > DialingPlanListType;

///
class DigitCollector
{
    public:
		DigitCollector(int deviceId, Sptr < Fifo < Sptr < SipProxyEvent > > > cpq );

		void reset();     // Restart timer
		void clear();     // Stop timer

		void addDigit( char digit );
		//added by lijie
		void addPbxDigit( char digit );
		//added ends here

		bool isValidDial();
		/* added by lizhijie */
		bool isValidPbxDial();
		/* end of added */

		bool isDialTimeout();

		string getDial();
		const string& getUrl();

		string getDialedType()
		{
			return dialedType;
		}

		DialMethodsType getDialMethod()
		{
			return dialMethod;
		}


		////////////////////////////

		void setDestHostIpAddr(string& ipAddr)
		{
			destHostIpAddr = ipAddr;
		}
		void setSipPortNum(string& sipPort )
		{
			sipPortNum = sipPort;
		}

#if 0
		void addDialPlan(const DialingPlan* dialingPlan);
		void addSpeedDial(const string& key, const string& phoneNum);
		/* added by lizhijie */
		void addLocalDial(const string& key, const string& phoneNum);

		/* added by wuaiwu, 2005/4/12 */
		void addOthsDial(int type, const string& key, const string& index);
#endif

		void addIncomingUrl( const string& url );
		void setInitTimeout(int timeout)
		{
			interDigitTimeout->setInitTimeout(timeout);
		};
		void setInterDigitTimeout(int timeout)
		{
			interDigitTimeout->setInterDigitTimeout(timeout);
		};



		void setDialedUrl(const string& url);


		static	DialMethodsType checkDialMethod(const string& url);

	protected:

	private:
		
		void getUrlFromLocalDial();
		void getUrlFromSpeedDial();
		void getUrlFromIpAddr();
		void getUrlFromPhoneNumDial();
		void getUrlFromSpecKeyDial();
		void getCallReturnUrl();
		void getUrlFromDirectDial();
		void getPstnUrl();
		DialMethodsType findDialPlan();
		void setRegular();
		void flushDialPlan();
		


		static RegularContainer Regulars;
		static LocalDialMapType& localDialMap;
		static bool bHasLoad;
			
		static SpeedDialMapType& speedDialMap;
		static DialingPlanListType& dialingPlanList;
		static Sptr < Fifo < Sptr < SipProxyEvent > > > callProcessingQueue;
		static const unsigned int MAX_PREV_IN_URLS_SIZE;
		static OthsDialMapType& 	recallDialMap;
		static OthsDialMapType&     specialKeyDialMap;
		static string destHostIpAddr;
		   
		static string sipPortNum;
		
		/* added lzj 2007.03.09 */
		static string	proxyPortNum;
		
		static DigitTimeout* interDigitTimeout;


		bool isPbxDial;	
		/* dial msg currently */
		string dial;					/* holding the dial digits */
		DialMethodsType dialMethod;
		string dialedType;
		string myDialedUrl;

		//stores the url of the last few incoming calls
		/* modify by wuaiwu, 2005/4/12. deque support random asscessing but list can't */
		deque <string> prevInUrls;

		/* added by lizhijie, 2005.02.11 */
		int myDeviceId;
		Sptr <PendingDigitTimerEvent> myPending;
	//static DialMethodsType currentStage;
};

}

#endif




#if 0

#ifndef DigitCollector_HXX
#define DigitCollector_HXX

/* ====================================================================
 * The Vovida Software License, Version 1.0 
 * 
 * Copyright (c) 2000 Vovida Networks, Inc.  All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 
 * 3. The names "VOCAL", "Vovida Open Communication Application Library",
 *    and "Vovida Open Communication Application Library (VOCAL)" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written
 *    permission, please contact vocal@vovida.org.
 *
 * 4. Products derived from this software may not be called "VOCAL", nor
 *    may "VOCAL" appear in their name, without prior written
 *    permission of Vovida Networks, Inc.
 * 
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL VOVIDA
 * NETWORKS, INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT DAMAGES
 * IN EXCESS OF $1,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 * 
 * ====================================================================
 * 
 * This software consists of voluntary contributions made by Vovida
 * Networks, Inc. and many individuals on behalf of Vovida Networks,
 * Inc.  For more information on Vovida Networks, Inc., please see
 * <http://www.vovida.org/>.
 *
 */

/*$Id: DigitCollector.hxx,v 1.6 2007/05/13 20:49:37 lizhijie Exp $*/

#include <locale.h>
#include <sys/types.h>
#ifdef _WIN32
#define __STDC__ 1
extern "C" {
#include <regex.h>
}
#else
#include <regex.h>
#endif
#include <string>
#include <deque>
#include <map>

#include <Sptr.hxx>
#include <SipProxyEvent.hxx>

#include "DigitTimeout.hxx"

namespace Vocal
{

class AssistDevice;

const string dial_ip = "IP";
const string dial_phone = "phone";

typedef map < string, string > SpeedDialMapType;

/* map of device name and INTERNAL_IP_DIAL phone number 
added by lizhijie, 2005.02.14 */
typedef map < string, string > LocalDialMapType;
typedef LocalDialMapType PstnDialMapType;
/* end of added */

/* added by wuaiwu, 2005/4/12. map for return call */
typedef map<string, string> OthsDialMapType;
typedef map<string, string> TransferMapType;
typedef map<string, string>::iterator TITER;
/* end of added */


typedef enum {
    WRONG_DIAL = -1,
    SPEED_DIAL,
    SPECIAL_KEY_DIAL,
    NORMAL_DIAL,
    INTERNAL_IP_DIAL,
    CALL_RETURN_DIAL,
    URL_DIAL,
    PSTN_DIAL,
    INCOMPLETE_DIAL
} DialMethodsType;

#if 0 //modified by lijie 2005-07-05
class DialingPlan
{
    public:
        DialingPlan(string pattern, DialMethodsType method);

        DialingPlan(const DialingPlan& src);

        bool operator ==(const DialingPlan& dialPlan ) const;
        void operator =(const DialingPlan& src);
        DialMethodsType getDialMethod() const
        {
            return dialMethod;
        };
        const string& getDialPattern() const
        {
            return dialPattern;
        };
    private:
        string dialPattern;
        DialMethodsType dialMethod;
};

typedef list < const DialingPlan* > DialingPlanListType;

///
class DigitCollector
{
    public:
        DigitCollector(int deviceId, Sptr < Fifo < Sptr < SipProxyEvent > > > cpq );

        void reset();     // Restart timer
        void clear();     // Stop timer

        void addDigit( char digit );
	//added by lijie
	    void addPbxDigit( char digit );
	//added ends here

        bool isValidDial();
		/* added by lizhijie */
		bool isValidPbxDial();
		/* end of added */
		
        bool isDialTimeout();

        string getDial();
        string getUrl();

        void setDestHostIpAddr(string& ipAddr)
        {
            destHostIpAddr = ipAddr;
        };
        void setSipPortNum(string& sipPort )
        {
            sipPortNum = sipPort;
        };
        void addDialPlan(const DialingPlan* dialingPlan);
        void addSpeedDial(const string& key, const string& phoneNum);
		/* added by lizhijie */
		void addLocalDial(const string& key, const string& phoneNum);

	 /* added by wuaiwu, 2005/4/12 */
	 void addOthsDial(int type, const string& key, const string& index);
        	
        void addIncomingUrl( const string& url );
        void setInitTimeout(int timeout)
        {
            interDigitTimeout->setInitTimeout(timeout);
        };
        void setInterDigitTimeout(int timeout)
        {
            interDigitTimeout->setInterDigitTimeout(timeout);
        };

        string getDialMethod()
        {
            return dialedType;
        };

	void setDialedUrl(const string& url);
	
		AssistDevice*  checkHardwareForLocalDial( string dial_phone_number );
		string getPhoneNumber( int deviceId );

	protected:

	private:

	 string getUrlFromLocalDial();

        string getUrlFromSpeedDial();
        string getUrlFromIpAddr();
        string getUrlFromPhoneNumDial();
        string getUrlFromSpecKeyDial();
        string getCallReturnUrl();

		
        void flushDialPlan();
        DialMethodsType findDialPlan();

	/* dial msg currently */
        string dial;					/* holding the dial digits */
        bool isNewDigit;
        DialMethodsType dialMethod;
        unsigned int maxDialPatternLen;
        string dialedType;

	/* config info */
	
		/* adde by lizhijie,2005.02.14 */
		LocalDialMapType localDialMap;
		/* end of added */

	/* add by wuaiwu, 2005/4/12 */

	 OthsDialMapType 	recallDialMap, specialKeyDialMap;
	
	/* end of added */
	
       SpeedDialMapType speedDialMap;
        DialingPlanListType dialingPlanList;
        string destHostIpAddr;
        string sipPortNum;
	string myDialedUrl;

        //stores the url of the last few incoming calls
        /* modify by wuaiwu, 2005/4/12. deque support random asscessing but list can't */
        deque < string > prevInUrls;

        //inter-digit timeout fifo
//        Sptr < DigitTimeout > interDigitTimeout;
		DigitTimeout* interDigitTimeout;
		
        ///
        Sptr < Fifo < Sptr < SipProxyEvent > > > callProcessingQueue;

        static const unsigned int MAX_PREV_IN_URLS_SIZE;

		/* added by lizhijie, 2005.02.11 */
		int myDeviceId;
		Sptr <PendingDigitTimerEvent> myPending;
		
        /*add by Tangbiao 2005-4-4*/	
     private:
	void reload_AllPlan();
	void flushSpeedDial();
        void flushLocalDial();
        /*add by Tangbiao 2005-4-4*/
        void show();
        /*add by Tangbiao 2005-4-8*/
	VMutex local_config_Mutex;		
};
#else

typedef struct
{
	bool isFullMatch;
	DialMethodsType method;
	regex_t reg;
}Regular;

typedef list< Sptr<Regular> > RegularContainer;

class DialingPlan
{
    public:
        DialingPlan(string pattern, DialMethodsType method);

        DialingPlan(const DialingPlan& src);

        bool operator ==(const DialingPlan& dialPlan ) const;
        void operator =(const DialingPlan& src);
        DialMethodsType getDialMethod() const
        {
            return dialMethod;
        };
        const string& getDialPattern() const
        {
            return dialPattern;
        };
    private:
        string dialPattern;
        DialMethodsType dialMethod;
};

typedef list < const DialingPlan* > DialingPlanListType;

///
class DigitCollector
{
    public:
        DigitCollector(int deviceId, Sptr < Fifo < Sptr < SipProxyEvent > > > cpq );

        void reset();     // Restart timer
        void clear();     // Stop timer

        void addDigit( char digit );
	//added by lijie
	    void addPbxDigit( char digit );
	//added ends here

        bool isValidDial();
		/* added by lizhijie */
		bool isValidPbxDial();
		/* end of added */
		
        bool isDialTimeout();

        string getDial();
        string getUrl();

        string getDialedType()
        {
            return dialedType;
        };

     	DialMethodsType getDialMethod()
        {
            return dialMethod;
        };
	 
	void setDialedUrl(const string& url);

#if 0	
		AssistDevice*  checkHardwareForLocalDial( string dial_phone_number );
		string getPhoneNumber( int deviceId );	

#endif

	static	DialMethodsType checkDialMethod(const string& url);

	void addIncomingUrl( const string& url );
   
	protected:

	private:
        void setDestHostIpAddr(string& ipAddr)
        {
            destHostIpAddr = ipAddr;
        };
        void setSipPortNum(string& sipPort )
        {
            sipPortNum = sipPort;
        };
        void addDialPlan(const DialingPlan* dialingPlan);
        void addSpeedDial(const string& key, const string& phoneNum);
		/* added by lizhijie */
		void addLocalDial(const string& key, const string& phoneNum);

	 /* added by wuaiwu, 2005/4/12 */
	 void addOthsDial(int type, const string& key, const string& index);
        	
     
        void setInitTimeout(int timeout)
        {
            interDigitTimeout->setInitTimeout(timeout);
        };
        void setInterDigitTimeout(int timeout)
        {
            interDigitTimeout->setInterDigitTimeout(timeout);
        };

		
	 string getUrlFromLocalDial();
        string getUrlFromSpeedDial();
        string getUrlFromIpAddr();
        string getUrlFromPhoneNumDial();
        string getUrlFromSpecKeyDial();
        string getCallReturnUrl();
        string getPstnUrl();
        DialMethodsType findDialPlan();
	 void setRegular();
	void flushDialPlan();
		


	static RegularContainer Regulars;
	static LocalDialMapType localDialMap;
	static bool bHasLoad;
			
       static SpeedDialMapType speedDialMap;
       static DialingPlanListType dialingPlanList;
	static Sptr < Fifo < Sptr < SipProxyEvent > > > callProcessingQueue;
	static const unsigned int MAX_PREV_IN_URLS_SIZE;
	static OthsDialMapType 	recallDialMap;
	static OthsDialMapType specialKeyDialMap;
       static string destHostIpAddr;
       static string sipPortNum;
	static DigitTimeout* interDigitTimeout;


	bool isPbxDial;	
	/* dial msg currently */
        string dial;					/* holding the dial digits */
        DialMethodsType dialMethod;
        string dialedType;
	string myDialedUrl;

        //stores the url of the last few incoming calls
        /* modify by wuaiwu, 2005/4/12. deque support random asscessing but list can't */
        deque < string > prevInUrls;

		/* added by lizhijie, 2005.02.11 */
		int myDeviceId;
		Sptr <PendingDigitTimerEvent> myPending;
};

#endif
}

#endif
#endif 

