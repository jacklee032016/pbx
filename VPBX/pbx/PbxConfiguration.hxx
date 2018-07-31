#ifndef __PBX_CONFIGURATION_HXX__
#define __PBX_CONFIGURATION_HXX__

/*
* $Id: PbxConfiguration.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $ 
*/

#include <string>
#include <vector>
#include <list>
#include <map>
#include "assist_lib.h"
#include "vpbx_globals.h"
using std::vector;
using std::string;
using std::map;

namespace Assist
{
typedef enum {
    XferOff = 0,
    XferOn = 1,
    ConfOn = 2
} UaXferMode;
typedef map<string, string> string_pair;
typedef string_pair OthsDialMapType;
typedef	string_pair SpeedDialMapType;
typedef map<string, bool> CallWaitMapType;
typedef	string_pair LocalDialMapType;
typedef string_pair UserInfoMap;
//const string dial_ip = "IP";
//const string dial_phone = "phone";

typedef map < string, string > SpeedDialMapType;

typedef map < string, string > LocalDialMapType;

typedef enum 
{
	WRONG_DIAL = -1,
	SPEED_DIAL ,				/* 0 */
	INTERNAL_DIAL ,				/* 1, dialing internal extension of PSTN and ISDN */
	NORMAL_DIAL ,				/* 2 */
	INTERNAL_IP_DIAL ,			/* 3 */
	CALL_RETURN_DIAL ,			/* 4 */
	URL_DIAL ,					/* 5 */
	DIRECT_DIAL ,				/* 6 */
	MISC_DIAL,					/* 7 , keep for compatible with VOIP */
	MISC2_DIAL,					/* 8 , keep for compatible with VOIP */
	PSTN_DIAL ,					/* 9 , PSTN external call */
	ISDN_DIAL ,					/* 10,ISDN external call */
	CONSOLE_DIAL,				/* 11 : call transfered into CONSOLE */
	EXTENSION_DIAL,			/* 12 : dialing internal extension of PSTN and ISDN */
	

	CONTINUE_CHECK_DIAL,		/* 13 : access code is right, but regex check is need next */
	INCOMPLETE_DIAL
} DialMethodsType;


typedef enum
{
	FXO_DIAL_PUSHBUTTON 	= 0,			/* Push Button, eg, DTMF */
	FXO_DIAL_DP10,
	FXO_DIAL_DP20
}FxoDialMode;

typedef enum
{
	FXO_LINE_DIRECT = 0,
	FXO_LINE_INDIRECT					/* after hearing DT from carrier switch, user need dial again */	
}FxoLineMode;

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
		string 					dialPattern;
		DialMethodsType 			dialMethod;
};

typedef list < const DialingPlan* > DialingPlanListType;

typedef list <string>  	PstnJumpCodeList;			/* PSTN special number which is used as connect phone to external line */


typedef map < string, string > AccessCodeMap;

typedef map < string, string > PlugInsMap;

/* 'string' is access_code for a deviceMgr , and all ext are place into vector */
typedef map < string,  list<string> > 	ExtMap;

class PbxConfiguration
{
	public:
		//    static const string DefaultRtpPort;

		static PbxConfiguration* instance();
		static void instance( const string cfgFile );

		static void parseCfgFileCallBack( char* tag, char* type, char* value );

		void show();

		string getUserName();
		string getPassword();
		string getDisplayName();

		int getInitialDigitTimeout();	/* ms */
		int getInterDigitTimeout();	/* ms */

		DialingPlanListType& getDialPlans();
		SpeedDialMapType& getSpeedDials();

		void setUserName( string name );  
		void setPassword( string pwd );
		void setDisplayName( string name );
		void setInitialDigitTimeout( int timeout /* ms */); 
		void setInterDigitTimeout( int timeout /* ms */);

		void addDialPattern( DialMethodsType type, string pattern );
		void addSpeedDial( string key, string dest );
		
		/* added by lizhijie */
		void addLocalDial( string key, string dest );
		LocalDialMapType& getLocalDials();

		void addUserInfo(string key,string dest);
		UserInfoMap& getUserInfo();

		void setPbxCount(int count)
		{
			pbxCount = count;	
		};
		
		int getPbxCount()
		{
			return pbxCount;
		};

		void setRxGain(float gain)
		{
			rxGain = gain;	
		};
		
		float getRxGain()
		{
			return rxGain;
		};

		void setTxGain(float gain)
		{
			txGain = gain;	
		};
		
		float getTxGain()
		{
			return txGain;
		};

		void setPbxDtmfMode(int mode)
		{
			pbxDtmfMode = mode;	
		};
		
		int getPbxDtmfMode()
		{
			return pbxDtmfMode;
		};

		void setPrevUrlSize(int size)
		{
			prevUrlSize = size;	
		};
		
		int getPrevUrlSize()
		{
			return prevUrlSize;
		};

		void setLogLevel(int level)
		{
			if(level>=0 && level<= 10)
				logLevel = level;	
			else
				logLevel = 4; /* default DEBUG_WARNNING */
		};
		
		int getLogLevel()
		{
			return logLevel;
		};

		const char *getPhoneNumberByDeviceName(char *devicename);


		int getAnswerRestartTimer();
		void setAnswerRestartTimer( int delay );

		string getDeviceName() const;
		void setDeviceName( const string devicename );

#if 0
		bool getCallWaitingOn(string phone);
		void setCallWaitingOn( const string phone, const bool callwaiting );
#endif
		 void addVMServer(const string server);
        	vector < string > & getVMServers();
		void setCallIdOn( const bool callIdOn );
		bool isCallIdOn();

		void setCallId( const string callid );
		string getCallId();

		pstn_standard_t getPstnStandard();
		void setPstnStandard(pstn_standard_t pstn_standard_t);

		void setCarTimes(int car);
		int getCarTimes(void);

		void setCarCount(int car);
		int getCarCount(void);

		void setIsdnTimeout( const int timeout );
		int getIsdnTimeout() const;

		void setLogFilename(const string& filename);
		string getLogFilename() const;

		void setGreetingFileName(const string& filename);
		string getGreetingFileName() const;

		
		void setPowerOnCall(int on)
		{
			this->powerOnCall = on;
		};

		int getPowerOnCall()
		{
			return powerOnCall;
		};


		int	getAccessLength();
		void setAccessLength(int length);

#if 0
		const char *getIsdnAccessCode();
		const char *getPstnAccessCode();
		const char *getVoipAccessCode();
		const char *getConsoleAccessCode();
#endif		

		const AccessCodeMap &getAccessCodeMap();
		const char *getAccessCode(const char *plugIn);
		void setAccessCode( string plugIn, string code);


		void addPlugIn( string _name, string libName );
		PlugInsMap&  getPlugIns();

		 vector < string > myVmServers;
		int	getExtLength();
		void setExtLength(int length);
		const char *getExtAccessCode();
		void setExtAccessCode(char *code);
		
		void addExtPhoneNumber(string access_code, string phone_number);
		const ExtMap &getExtMap();

		const char *getIsdnLocalDialCode();
		void setIsdnLocalDialCode(const char *_localDialCode);

		const char *getIsdnExternalDialCode();
		void setIsdnExternalDialCode(const char *_externalDialCode);

		int getConsoleCallingTimeout();				/* ms */
		void setConsoleCallingTimeout( int timeMs);
		int getConsoleAlertingTimeout();				/* ms */
		void setConsoleAlertingTimeout(int timeMs);

		FxoLineMode  getFxoLineMode();
		void setFxoLineMode(FxoLineMode dialLineMode);
		
		FxoDialMode  getFxoDialMode();
		void setFxoDialMode(FxoDialMode dialMode);
		
		PstnJumpCodeList&  getPstnJumpCodeList();
		void addPstnJumpNumber(string jumpNumber);

		int getDtmfMarkTime();
		void setDtmfMarkTime(int _time);
		int getDtmfDurationTime();
		void setDtmfDurationTime(int _time);

		as_dsp_config_t *getDspConfig();	

		string getLocalSipPort();
		void setLocalSipPort(string name);
		string getSipTransport();
		void setSipTransport( string name);
		void setRegisterOn( const bool registeron );
		bool getRegisterOn();
		void setRegisterServer(string name);
		string getRegisterServer();
		void setProxyServer(string name);
		string getProxyServer();
		void setRegisterExpires(int expirestime);
		int getRegisterExpires();
		void setRegisterAction(string name);
		string getRegisterAction();
		void setMinRTPPort( int minport);
		int getMinRTPPort();
		void setMaxRTPPort(int maxport);
		int getMaxRTPPort();
		void setVadString(const bool vadstring);
		bool getVadString();
		void setJitterBuffer(int jitterbuffer);
		int getJitterBuffer();
		void setStartCdrServer(int cdrserver);
		int getStartCdrServer();
		void setAuthentication(bool isCheck);
		bool getAuthentication();
		void setSubscribeOn( const bool subOn );
        	bool getSubscribeOn() ;
		void setSubscribeExpires(int subscribe);
		int getSubscribeExpires();
		void setSubscribeTo(string name);
		string getSubscribeTo();
		void setCdrPath(string name);
		string getCdrPath();
		void setFifo(string name);
		string getFifo();
		void setG711Law(int glaw);
		int getG711Law();
		void setRsvpOn(const bool rsvp);
		bool getRsvpOn();
		void setRegisterFrom( string from );
		string getRegisterFrom();
		void setRegisterTo( string to );
		string getRegisterTo();
		void setNATAddress(const string& address);
       	string getNATAddress() const;
		void setNetworkRtpRate( int rate );
		int getNetworkRtpRate();
		void setXferMode( const UaXferMode Xfermode );
		UaXferMode getXferMode() const;
		void setConfServer(const string confser);
		string getConfServer() const;
		void setProvideRingback( bool ringback );
		bool getProvideRingback();
		
		void setUaTimeout( const int timeout );
		int getUaTimeout();
		
		const string& getUserPwd(const string& name)
		{
			return users[name];
		}
		void setUsePolicyServer( const bool use );
		bool getUsePolicyServer() const;
		string getLocalExchange() const;
		void setLocalExchange(string _localExchange);
		void setLocalCountry(string _localCountry);
		string getLocalCountry() const;
		void setLocalDistrict(string _localDistrict);
		string getLocalDistrict() const;
		void setLocalDevice(string _localDevice)	;
		string getLocalDevice() const;
		void setLocalBranch(string _localBranch);
		string getLocalBranch() const;
		void addOthsDial(int type,const string& key, const string& dest);
		OthsDialMapType& getOthsDials(int type);
		void setProvisionHost(string prohost);
		string getProvisionHost() const;
		void setProvisionPort(int proport);
		int getProvisionPort() const;

		~PbxConfiguration();

	protected:


	private:
		PbxConfiguration();
		void write_pid_tofile();

		string 					userName;
		string 					displayName;
		string 					password;

		bool 					provideRingback; 
		int 						initialDigitTimeout;
		int 						interDigitTimeout;

		DialingPlanListType 		dialPlans;
		SpeedDialMapType 		speedDials;
		/* adde by lizhijie,2005.02.14 */
		LocalDialMapType 			localDials;
		
		int 						pbxCount;
		int 						logLevel;
		float 					rxGain;
		float 					txGain;
		int 						pbxDtmfMode;
		int 						prevUrlSize;

		int 						answerRestartTimer;

		string 					deviceName;

		string 					myCallId;
		bool 					myCallIdOn; 

		pstn_standard_t			pstnStandard;
		
		/* CAR timeout for FXS device */
		int						carTimes;
		/* CAR count for FXO port */
		int						carCount;

		int 						isdnTimeout;

		string 					myLogFilename;
		string					myGreetingFileName;

		int 						powerOnCall;

		
		int						accessLength;
#if 0		
		char			isdnAccess[MAX_ACCESS_CODE_LENGTH];
		char			pstnAccess[MAX_ACCESS_CODE_LENGTH];
		char			voipAccess[MAX_ACCESS_CODE_LENGTH];
		char			consoleAccess[MAX_ACCESS_CODE_LENGTH];
#endif
		AccessCodeMap			accessCodeMap;

		PlugInsMap				plugIns;

		int						extLength;
		const char				extAccessCode[32];
		ExtMap					extMaps;

		string					myIsdnLocalDialCode;
		string					myIsdnExternalDialCode;

		int 						consoleCallingTimeout;
		int 						consoleAlertingTimeout;

		FxoLineMode				fxoLineMode;
		FxoDialMode				fxoDialMode;
		PstnJumpCodeList			jumpList;

		int 						dtmfMarkTime;
		int						dtmfDurationTime;

		as_dsp_config_t			dspConfig;
		
		string					sipport;
		string					siptransport;
		bool						registeron;
		string					registerserver;
		string					proxyserver;
		int						registerexpires;
		string					registeraction;
		int 						minrtpport;
		int 						maxrtpport;
		bool						vadstringpbx;
		int						jitterbufferpbx;
		int						startcdrserver;
		
		bool						isAuthentication;
		bool						subscribeon;
		
		int 						subscribeexpires;
		
		string					subscribeto;
		string					cdrpath;
		string 					fifo;
		int						g711law;
		bool						rsvpon;
		string					registerFrom;
		string					registerTo;
		string					myNATAddress;
		int						networkRtpRate;
		UaXferMode				xferMode;
		string					confserver;
		
		int						uaTimeout;

		UserInfoMap 				users; 
		bool 					usePolicyServer;
		string					localExchange;
		string					localCountry;
		string					localDistrict;
		string					localDevice;
		string					localBranch;
		OthsDialMapType 			recallDials;
		OthsDialMapType 			spKeyDials;
		string					ProHost;
		int						ProPort;
		
		static PbxConfiguration* 	isdnConfiguration;

		void displayExts();
		
};
 
}

#endif
