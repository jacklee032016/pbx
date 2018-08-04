#ifndef UA_CONFIGURATION_HXX
#define UA_CONFIGURATION_HXX

/* $Id: UaConfiguration.hxx,v 1.7 2007/09/08 18:21:41 lizhijie Exp $ */

#include <string>
#include <vector>

#include "DigitCollector.hxx"

#include "pbx_globals.h"

typedef map<string, bool> CallWaitMapType;


namespace Vocal
{
typedef OthsDialMapType UserInfoMap;

typedef enum {
    XferOff = 0,
    XferOn = 1,
    ConfOn = 2
} UaXferMode;


class UaConfigurationErr : public VException
{
public:
	UaConfigurationErr(const string& reason, 
				    const string& file,
				    const int line);
	string getName() const; // { return "UaConfigurationErr"; }
};

/**
 *  SIP User Agent configuration
 */
class UaConfiguration
{
    public:
        ///
        //    static const string DefaultRtpPort;
        ///
        static UaConfiguration* instance();
        ///
        static void instance( const string cfgFile );
        ///
        static void parseCfgFileCallBack( char* tag, char* type, char* value );
        ///
        void show();

#if 0
        /*add by tangbiao 2005-4-4*/
	 int getflag_change();
	 int getflag_alter();
	 void resetflag_alter();
	 VMutex config_Mutex;	
         void lock_config();
	 void unlock_config();
	 void signal_config();
         void reload_config();
	/*add by tangbiao 2005-4-4*/
	static void reload_parseCfgFileCallBack( char* tag, char* type, char* value );
	/*add by tangbiao 2005-4-11*/
#endif
	
	
        ///
        string getUserName();
        ///
        string getPassword();
        ///
        string getDisplayName();
        ///
        string getLocalSipPort();
        ///
        int getMinRtpPort();
        ///
        int getMaxRtpPort();
        ///
        string getSipTransport();
        ///
        string getProxyServer();

        bool getRegisterOn();

        string getRegisterFrom();

        string getRegisterTo();

		const string& getProxyPort() const;
		const string& getRegisterPort() const;

        int getRegisterExpires();
        ///
        string getRegisterAction();
        ///
        int getNetworkRtpRate();
        ///
        bool getProvideRingback();
        ///
        int getInitialDigitTimeout();
        ///
        int getInterDigitTimeout();
        ///
        UaXferMode getXferMode() const;
        ///
        string getConfServer() const;
        ///
        DialingPlanListType& getDialPlans();
        ///
        SpeedDialMapType& getSpeedDials();

	//added by lijie
	string getLocalExchange() const;
	void setLocalExchange(string _localExchange);

	string getBusyTransfer(string key);
	void setBusyTransfer(string key, string value );
	string getTimeOutTransfer(string key);
	TransferMapType& getTimeOutTransfers();
	TransferMapType& getAllTransfers();
	TransferMapType& getBusyTransfers();
	void setTimeOutTransfer(string key, string value);
	string getAllTransfer(string key);
	void setAllTransfer(string key, string value);
	//added ends here
        ///
        void setUserName( string name );
        ///
        void setPassword( string pwd );
        ///
        void setDisplayName( string name );
        ///
        void setLocalSipPort( string port );
        ///
        void setMinRtpPort( int port );
        ///
        void setMaxRtpPort( int port );
        ///
        void setSipTransport( string transport );
        ///
        void setProxyServer( string server );
        ///
        void setRegisterOn( bool on );
        ///
        void setRegisterFrom( string from );
        ///
        void setRegisterTo( string to );
        ///
        void setRegisterExpires( int expires );
        ///
        void setRegisterAction( string action );

		void setRegisterPort(string _port);
		void setProxyPort(string _port);
		
        void setNetworkRtpRate( int rate );
        ///
        void setProvideRingback( bool ringback );
        ///
        void setInitialDigitTimeout( int timeout );
        ///
        void setInterDigitTimeout( int timeout );
        ///
        void setXferMode( const UaXferMode xfermode );
        ///
        void setConfServer( const string confserver );

        ///
        void addDialPattern( DialMethodsType type, string pattern );
        ///
        void addSpeedDial( string key, string dest );
		
		/* added by lizhijie */
		void addLocalDial( string key, string dest );
		LocalDialMapType& getLocalDials();

		void addPstnDial( string key, string dest );
		PstnDialMapType& getPstnDials();
	 /* added by wuaiwu, 2005/4/12 */

		void addPstnHwStatus( int index, bool isOn );
		PstnHwStatusMap& getPstnHwStatusMap();

	void addOthsDial(int type,const string& key, const string& dest);
	OthsDialMapType& getOthsDials(int type);
		
	/* end of added */

		void setPbxCount(int count)
		{
			pbxCount = count;	
		};
		
		int getPbxCount()
		{
			return pbxCount;
		};

		void setRxGain(int gain)
		{
			rxGain = gain;	
		};
		
		int getRxGain()
		{
			return rxGain;
		};

		void setTxGain(int gain)
		{
			txGain = gain;	
		};
		
		int getTxGain()
		{
			return txGain;
		};

		void setG711Law(as_law_type_t _law)
		{
			g711Law = _law;	
		};
		
		as_law_type_t getG711Law()
		{
			return g711Law;
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

		/* added lizhijie, 2007.03.20 */
		void setLogSize(int size);
		
		int getLogSize();
		/* end of added by lizhijie */

		const char *getPhoneNumberByDeviceName(char *devicename);
             assist_app_type  getAppType() const;
		
		/* end of added */

		int getAnswerRestartTimer();
		void setAnswerRestartTimer( int delay );

		//added by lijie
		void setMaxDropTime(int time);
		int getMaxDropTime() const;
		void setWatchDogTime(int time);
		int getWatchDogTime() const;
		 void setPstnCount( int timeout );
        	int getPstnCount() const;

		void setLocalCountry(string _localCountry)
		{
			localCountry = _localCountry;
		}

		string getLocalCountry() const
		{	
			return localCountry;
        	}	

		void setLocalDevice(string _localDevice)
		{
			localDevice = _localDevice;
		}

		string getLocalDevice() const
		{	
			return localDevice;
        	}	

		/*added 2007.03.08 */
		void setConsolePhone(string _phoneNumber)
		{
			consolePhone = _phoneNumber;
		}

		string getConsolePhone() const
		{	
			return consolePhone;
        	}	

		/*added 2007.03.27 */
		void setGreetingFile(string _greetingFile)
		{
			greetingFile = _greetingFile;
		}
		string getGreetingFile() const
		{	
			return greetingFile;
        	}	
		
		void setLocalDistrict(string _localDistrict)
		{
			localDistrict = _localDistrict;
		}

		void setLocalBranch(string _localBranch)
		{
			localBranch = _localBranch;
		}

		string getLocalBranch() const
		{	
			return localBranch;
        	}	
		
		string getLocalDistrict() const
		{	
			return localDistrict;
        	}	
		//added ends here
        ///
        bool getRsvpOn() const;
        ///
        string getProvisioningHost() const;
        ///
        int getProvisioningPort() const;
        ///
        bool getUsePolicyServer() const;
        ///
        string getDeviceName() const;
        ///
        bool getCallWaitingOn(string phone);

	CallWaitMapType& getCallWaits();

        ///
        void setRsvpOn( const bool on );
        ///
        void setProvisioningHost( const string host );
        ///
        void setProvisioningPort( const int port );
        ///
        void setUsePolicyServer( const bool use );
        ///
        void setDeviceName( const string devicename );
        ///
        void setCallWaitingOn( const string phone, const bool callwaiting );
        ///
        void setCallIdOn( const bool callIdOn );
        ///
        bool getCallIdOn();
        ///
        void setCallId( const string callid );
        ///
        string getCallId();
        ///
        void setUaTimeout( const int timeout );
        ///
        int getUaTimeout() const;
        ///
        void setSubscribeOn( const bool subOn );
        ///
        bool getSubscribeOn() const;
        ///
        void setSubscribeExpire( const int expire );
        ///
        int getSubscribeExpire() const;
        ///
        void setSubscribeTo( const string to );
        ///
        string getSubscribeTo() const;

        ///
        void addVMServer(const string server);
        ///
        vector < string > & getVMServers();

        /// set the filename that log messages should be sent to
        void setLogFilename(const string& filename);
        /// get the filename that log messages should be sent to
        string getLogFilename() const;

        /** set the NAT address, used to fill in the c= and o= lines
            in the SDP, so that RTP audio can tunnel through a NAT
            firewall.
        */
        void setNATAddress(const string& address);
        /** get the NAT address */
        string getNATAddress() const;

		/// for VAD
		bool getVad();
		void setVad(bool flag);
		int getJitterBuffer();
		void setJitterBuffer(int jitterNew);
	 /* added by wuaiwu 2005/05/15 */
       /* inline these ... */
	 
	  int getCdrOn() const {return cdrOn;}
	  void setCdrOn(int flag){cdrOn = flag;}

	  string getFifo() const {return fifoName;}
	  void setFifo(const char* s){fifoName = s;}

	  string getCdr() const {return cdrName;}
	  void setCdr(const char* c) {cdrName = c;}
	 
	 /* end of added */
	 
/*added by wuaiwu 2005/06/29 */
         void setRegisterServer(const char* tag);
         const string& getRegisterServer() const;
/* end of added */
/* added by wuaiwu 2005/07/08 */

void addUserInfo(const string& name, const string& pwd)
{
	users[name] = pwd;
}


const string& getUserPwd(const string& name)
{
	return users[name];
}

void setAuthentication(const char* sAuthen)
{
	Authentication = sAuthen;
}

const string& getAuthentication() const
{
	return Authentication;
}


/* end of added*/
        ///
        ~UaConfiguration();

    protected:
        ///

    private:
        ///
        UaConfiguration();
 	  static UaConfiguration* LocalInit();
#if 0	
        /*add by Tangbiao 2005-4-4*/
        void flushDialPlan();
	void flushLocalDial();
	void flushSpeedDial();
	volatile int change_flag;
	string public_cfgFile;
	int alter_flag;
#endif
	/*add by Tangbiao 2005-4-4*/
	 void write_pid_tofile();
        /*add by tangbiao 2005-4-7*/	

	int watchDogTime;
	int maxDropTime;
	int pstnCount;
	string localCountry;
	string localDistrict;
	string localBranch;
	string localDevice;

		/* added 2007.03.08 */
		string	consolePhone;
		/* added 2007.03.27, lizhijie */
		string	greetingFile;
        ///
        string userName;
        ///
        string displayName;
        ///
        string password;
        ///
        string localSipPort;
        ///
        int minRtpPort;
        ///
        int maxRtpPort;
        ///
        string sipTransport;
        ///
        string proxyServer;
        ///
        bool registerOn;
        ///
        string registerFrom;
        ///
        string registerTo;
        ///
        int registerExpires;
        ///
        string registerAction;

		string registerPort;
		string proxyPort;
		
        int networkRtpRate;
        ///
        bool provideRingback;
        ///
        int initialDigitTimeout;
        ///
        int interDigitTimeout;
        ///
        DialingPlanListType dialPlans;
        ///
        SpeedDialMapType speedDials;

		/* adde by lizhijie,2005.02.14 */
		LocalDialMapType localDials;
		PstnDialMapType pstnDials;
		
		PstnHwStatusMap		pstnHwStatusMap;/*adde lizhijie, 2007.05.12 */
		
		int pbxCount;
		int	logLevel;
		int	logSize;/*added lizhijie, 2007.03.20 */
		
		int	rxGain;
		int	txGain;

		as_law_type_t g711Law;
		int pbxDtmfMode;
		int prevUrlSize;

		/* added 05.03.03 */
		assist_app_type  appType;
		/* end of added */


	 /* added by wuaiwu, 2005/4/12 */

		OthsDialMapType recallDials;
		OthsDialMapType spKeyDials;
			
	/* end of added */

        int answerRestartTimer;
	//added by lijie
	string localExchange;
	TransferMapType	busyTransfer;
	TransferMapType   timeoutTransfer;
	TransferMapType   allTransfer;
	//added ends here
        /**
         * RSVP configuration data
         */
        ///
        bool rsvpOn;
        ///
        string provisioningHost;
        ///
        int provisioningPort;
        ///
        bool usePolicyServer;

        /**
         * other parameters
         */
        ///
        string deviceName;
        ///
        UaXferMode xferMode;
        ///
        string confServer;
        ///
        string myCallId;
        ///
        bool myCallIdOn;
        ///
  #if 0      
        bool callWaitingOn;
  #else
  	CallWaitMapType callWait;
  #endif
        ///
        int uaTimeout;
        ///
        bool subscribeOn;
        ///
        int subscribeExpire;
        ///
        string subscribeTo;
        ///
        vector < string > myVmServers;

        string myLogFilename;
        string myNATAddress;

	///
	bool vadOn;
	///
	int jitterBuffer;

	/* add by wuaiwu 2005/05/15 */

	int cdrOn;
	string fifoName;
	string cdrName;

	/* end of added */
  
	 string registerServer; /*added by wuaiwu, 2005/06/29 */	 
        ///
        /* added by wuaiwu, 2005/07/08 */
        UserInfoMap users; 
        string Authentication;
        // end of added
	 
	 static int objcounts;
		
        static UaConfiguration* uaConfiguration;
};
 
}

#endif
