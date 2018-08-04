/*$Id: UaConfiguration.cxx,v 1.12 2007/09/08 18:21:40 lizhijie Exp $*/

#include "global.h"
#include <cassert>
#include <cstdio>
#include "cpLog.h"
#include "parse3tuple.h"

#include "Data.hxx"
#include "UaConfiguration.hxx"


using namespace Vocal;

int UaConfiguration::objcounts = 0;
UaConfiguration* UaConfiguration::uaConfiguration = UaConfiguration::LocalInit();


UaConfiguration *UaConfiguration::instance()
{
	assert( uaConfiguration != 0 );
	return uaConfiguration;
}    // UaConfiguration::instance


typedef enum
{
	UserNameTag = 0,
	DisplayNameTag,
	PasswordTag,
	LocalSipPortTag,
	MinRtpPortTag,                          
	MaxRtpPortTag,
	SipTransportTag,
	ProxyServerTag,
	RegisterOnTag,
	RegisterFromTag,
	RegisterToTag,
	RegisterServerTag,
	RegisterExpiresTag,
	RegisterActionTag,
	NetworkRtpRateTag,
	ProvideRingbackTag,
	InitialDigitTimeoutTag,
	InterDigitTimeoutTag,
	DialPatternTag,
	SpeedDialTag,

	/* added by lizhjie */    
	LocalDialTag ,
	CallReturnDialTag,    
	                   //added by wuaiwu, 2005/4/12
	SpecialKeyDialTag,    
	PbxCountTag ,
	LogLevelTag ,

	LogSizeTag,	/* lizhijie, 2007.03.20 */
	GreetingFileTag,	/* lizhijie, 2007.03.27 */
	
	RxGainTag,
	TxGainTag,
	G711LawTag,
	PbxDtmfModeTag,
	PrevUrlSizeTag,
	/* end of added */

	AnswerRestartTimerTag,
	//added by lijie
	LocalExchangeTag, 
	BusyTransferTag,
	TimeOutTransferTag,
	AllTransferTag,	
	MaxDropTimeTag,
	WatchDogTimeTag,
	PstnCountTag,
	PstnDialTag,

	PstnHwTag,		/* Li Zhijie 2007.05.12 */
	
	LocalBranchTag,
	LocalDistrictTag,
	LocalCountryTag,
	LocalDeviceTag,
	//lijie added ends here

	RsvpOnTag,
	ProvisioningHostTag,
	ProvisioningPortTag,
	UsePolicyServerTag,
	UaXferModeTag,
	ConfServerTag,
	DeviceNameTag,
	CallWaitingOnTag,
	CallIdOnTag,
	CallIdTag,
	UaTimeoutTag,
	SubscribeOnTag,
	SubscribeExpireTag,
	SubscribeToTag,
	VmServerTag,
	LogFileNameTag,
	NATAddressTag,
	VadTag,
	JitterBufferTag,
	CdrTag,
	CdrPathTag,
	FifoTag,
	UserInfoTag,
	AuthenticationTag,

	/* added 2007.03.05*/
	ConsolePhoneTag,
	ProxyPortTag,
	RegisterPortTag,
	
	UaConfigurationTagMax
} UaConfigurationTags;


static const char* UaConfigurationTagString [] =
{
		"User_Name",
		"Display_Name",
		"Pass_Word",
		"Local_SIP_Port",
		"Min_RTP_Port",
		"Max_RTP_Port",
		"SIP_Transport",
		"Proxy_Server",
		"Register_On",
		"Register_From",
		"Register_To",
		"Register_Server",
		"Register_Expires",
		"Register_Action",
		"Network_RTP_Rate",
		"Provide_Ringback",
		"Initial_Digit_Timeout",
		"Inter_Digit_Timeout",
		"Dial_Pattern",
		"Speed_Dial",        

		/* added by lizhijie */        
		"Local_Dial" , 
		"Call_Return",    /* added by wuaiwu */
		"Special_Key",   /* added by wuaiwu */
		"Pbx_Count" ,
		"Log_Level" ,

		"Log_Size" ,		/* added lizhijie,2007.03.20 */
		"Greeting_File",	/* added lizhijie,2007.03.27 */

		"Gain_RX",
		"Gain_TX",
		"G711_Law",
		"Pbx_DTMF_Mode",
		"Prev_URL_Size",
		/* end of added */	

		"AnswerRestartTimer",
		//added by lijie
		"Local_Exchange",
		"Busy_Transfer",
		"TimeOut_Transfer",
		"All_Transfer",	
		"Max_DropTime",
		"WatchDog_Time",
		"Pstn_Count",
		"Pstn_Dial",
		
		"Pstn_Hw_Status",

		"Local_Branch",
		"Local_District",
		"Local_Country",
		"Local_Device", 
		//added ends here

		"Rsvp_On",
		"Provisioning_Host",
		"Provisioning_Port",
		"Use_Policy_Server",
		"Ua_Xfer_Mode",
		"Conference_Server",
		"Device_Name",
		"Call_Waiting_On",
		"CallId_On",
		"CallId",
		"Ua_Timeout",
		"Subscribe_On",
		"Subscribe_Expires",
		"Subscribe_To",
		"VmServer",
		"LogFilename",
		"NATAddress",
		"VadString",
		"JitterBuffer",
		"Start_CdrServer",
		"Cdr_Path",
		"Fifo",
		"UserInfo",
		"Authentication",

		/* added 2007.03.08 */
		"IP_Console_Phone",
		"Proxy_Port",
		"Register_Port",
		"Unknown"
};

UaConfigurationErr::UaConfigurationErr(const string& reason, 
				                                       const string& file,
				                                       const int line)
	: VException(reason, file, line)
{
}
				 
string UaConfigurationErr::getName() const
{
	return "UaConfigurationErr";
}


void UaConfiguration::parseCfgFileCallBack( char* tag, char* type, char* value )
{
	switch ( strConst2i( tag, UaConfigurationTagString, UaConfigurationTagMax ) )
	{
		case UserNameTag:
		{
			UaConfiguration::instance()->setUserName( value );
		}
		break;

		case DisplayNameTag:
		{
			UaConfiguration::instance()->setDisplayName( value );
		}
		break;

		case PasswordTag:
		{
			UaConfiguration::instance()->setPassword( value );
		}
		break;

		case LocalSipPortTag:
		{
			UaConfiguration::instance()->setLocalSipPort( value );
		}
		break;

		case MinRtpPortTag:
		{
			int i;
			sscanf( value, "%d", &i );
			UaConfiguration::instance()->setMinRtpPort( i );
		}
		break;

		case MaxRtpPortTag:
		{
			int i;
			sscanf( value, "%d", &i );
			UaConfiguration::instance()->setMaxRtpPort( i );
		}
		break;

		case SipTransportTag:
		{
			UaConfiguration::instance()->setSipTransport( value );
		}
		break;

		case ProxyServerTag:
		{
			if(value && strncasecmp(value, "0.0.0.0", 7 ) )/*added this line, lizhijie, 2006.10.22 */
				UaConfiguration::instance()->setProxyServer( value );
		}
		break;

		case RegisterOnTag:
		{
			UaConfiguration::instance()->setRegisterOn( isEqualNoCase( string( value ), "True" ) );
		}
		break;

		case RegisterFromTag:
		{
			UaConfiguration::instance()->setRegisterFrom( value );
		}
		break;

		case RegisterToTag:
		{
			UaConfiguration::instance()->setRegisterTo( value );
		}
		break;

		case RegisterServerTag:
		{
			UaConfiguration::instance()->setRegisterServer(value);
		}
		break;
		
		case RegisterExpiresTag:
		{
			int i;
			sscanf( value, "%d", &i );
			UaConfiguration::instance()->setRegisterExpires( i );
		}
		break;

		case RegisterActionTag:
		{
			UaConfiguration::instance()->setRegisterAction( value );
		}
		break;

		case ProxyPortTag:
		{
			UaConfiguration::instance()->setProxyPort( value );
		}
		break;
		case RegisterPortTag:
		{
			UaConfiguration::instance()->setRegisterPort( value );
		}
		break;

		case NetworkRtpRateTag:
		{
			int i;
			sscanf( value, "%d", &i );
			UaConfiguration::instance()->setNetworkRtpRate( i );
		}
		break;

		case ProvideRingbackTag:
		{
			int flag;
			sscanf( value, "%d", &flag );
			UaConfiguration::instance()->setProvideRingback( isEqualNoCase( string( value ), "True" ) ) ;
		}
		break;

		case InitialDigitTimeoutTag:
		{
			int i;
			sscanf( value, "%d", &i );
			UaConfiguration::instance()->setInitialDigitTimeout( i );
		}
		break;

		case InterDigitTimeoutTag:
		{
			int i;
			sscanf( value, "%d", &i );
			UaConfiguration::instance()->setInterDigitTimeout( i );
		}
		break;

		case DialPatternTag:
		{
			int type;
			char pattern[ VALUE_MAX_LENGTH ];
			sscanf( value, "%d %s", &type, pattern );
			UaConfiguration::instance()->addDialPattern( (DialMethodsType)type, pattern );
		}
		break;

		case SpeedDialTag:
		{
			char key[ VALUE_MAX_LENGTH ];
			char dest[ VALUE_MAX_LENGTH ];
			sscanf( value, "%s %s", key, dest );
			UaConfiguration::instance()->addSpeedDial( key, dest );
		}
		break;

		/* added by lizhijie, 2005.02.14 */
		case LocalDialTag:
		{
			char key[ VALUE_MAX_LENGTH ];
			char dest[VALUE_MAX_LENGTH];
			char val[VALUE_MAX_LENGTH];
			sscanf( value, "%s %s", key, dest );

			sprintf(val, "%s_HARDWARE_%s", UaConfiguration::instance()->getDeviceName().c_str(), dest );
			UaConfiguration::instance()->addLocalDial( key, val );
		}
		break;

		case PstnDialTag:
		{
			char key[ VALUE_MAX_LENGTH ];
			char dest[VALUE_MAX_LENGTH];
			char val[VALUE_MAX_LENGTH];
			sscanf( value, "%s %s", key, dest );

			sprintf(val, "%s_PSTN_%s", UaConfiguration::instance()->getDeviceName().c_str(), dest );
			UaConfiguration::instance()->addPstnDial( key, val );
		}
		break;

		case PstnHwTag:
		{
			char key[ VALUE_MAX_LENGTH ];
			char dest[ VALUE_MAX_LENGTH ];
			sscanf( value, "%s %s", key, dest );
			bool isOn = isEqualNoCase(string( dest ), "True" );
			UaConfiguration::instance()->addPstnHwStatus(atoi(key), isOn);
		}
		break;

		/* added by wuaiwu, 2005/4/12 */

		case CallReturnDialTag:
		{
			char key[ VALUE_MAX_LENGTH ];

			char dest[ VALUE_MAX_LENGTH ];

			sscanf( value, "%s %s", key, dest );

			UaConfiguration::instance()->addOthsDial( 0, key, dest );
		}
		break;

		case SpecialKeyDialTag:
		{
			char key[ VALUE_MAX_LENGTH ];

			char dest[ VALUE_MAX_LENGTH ];

			sscanf( value, "%s %s", key, dest );

			UaConfiguration::instance()->addOthsDial( 1, key, dest );
		}
		break;
		/* end of added */

		/* added by wuaiwu 2005/05/15 */
		case CdrTag:
		{
			UaConfiguration::instance()->setCdrOn( atoi(value) );
		}
		break;

		case CdrPathTag:
		{
			UaConfiguration::instance()->setCdr(value);
		}
		break;

		case FifoTag:
		{
			UaConfiguration::instance()->setFifo(value);
		}
		break;

		/*  end of added 2005/05/15*/
		/* added by wuaiwu, 2005/07/08 */
		case UserInfoTag:
		{
			char key[ VALUE_MAX_LENGTH ];

			char dest[ VALUE_MAX_LENGTH ];

			sscanf( value, "%s %s", key, dest );

			UaConfiguration::instance()->addUserInfo(key, dest );
		}
		break;

		case AuthenticationTag:
		{
			UaConfiguration::instance()->setAuthentication(value);
		}
		break;

		/*end of added */
		/* added by lizhijie, 2005.02.19 */
		case PbxCountTag:
		{
			int i;
			sscanf( value, "%d", &i );
			UaConfiguration::instance()->setPbxCount( i );
		}
		break;

		/* added by lizhijie, 2005.02.22 */
		case LogLevelTag:
		{
			int i;
			sscanf( value, "%d", &i );
			UaConfiguration::instance()->setLogLevel( i );
		}
		break;

		/* added by lizhijie, 2007.03.20 */
		case LogSizeTag:
		{
			int i;
			sscanf( value, "%d", &i );
			UaConfiguration::instance()->setLogSize( i );
		}
		break;

		/* added by lizhijie, 2005.02.23 */
		case RxGainTag:
		{
			int f;
			sscanf( value, "%d", &f );
			UaConfiguration::instance()->setRxGain( f );
		}
		break;
		case TxGainTag:
		{
			int f;
			sscanf( value, "%d", &f );
			UaConfiguration::instance()->setTxGain( f );
		}
		break;
		case G711LawTag:
		{
			int i;
			sscanf( value, "%d", &i );
			UaConfiguration::instance()->setG711Law( (as_law_type_t) i );
		}
		break;
		case PbxDtmfModeTag:
		{
			int i;
			sscanf( value, "%d", &i );
			UaConfiguration::instance()->setPbxDtmfMode( i );
		}
		break;
		
		case PrevUrlSizeTag:
		{
			int i;
			sscanf( value, "%d", &i );
			UaConfiguration::instance()->setPrevUrlSize( i );
		}
		break;
		/* end of added by lizhijie 02.23 */

		case AnswerRestartTimerTag:
		{
			int i;
			sscanf( value, "%d", &i );
			UaConfiguration::instance()->setAnswerRestartTimer( i );
		}
		break;

		case RsvpOnTag:
		{
			UaConfiguration::instance()->setRsvpOn( isEqualNoCase(string( value ), "True" ) );
		}
		break;

		case ProvisioningHostTag:
		{
			UaConfiguration::instance()->setProvisioningHost( value );
		}
		break;

		case ProvisioningPortTag:
		{
			int i;
			sscanf( value, "%d", &i );
			UaConfiguration::instance()->setProvisioningPort( i );
		}
		break;

		case UsePolicyServerTag:
		{
			UaConfiguration::instance()->setUsePolicyServer( isEqualNoCase(string( value ), "True" ) );
		}
		break;

		case UaXferModeTag:
		{
			UaXferMode xfermode = XferOff;
			if ( string( value ) == "Transfer" )
			{
				xfermode = XferOn;
			}
			else if ( string( value ) == "Conference" )
			{
				xfermode = ConfOn;
			}
			UaConfiguration::instance()->setXferMode( xfermode );
		}
		break;

		case ConfServerTag:
		{
			UaConfiguration::instance()->setConfServer( string(value) );
		}
		break;

		case DeviceNameTag:
		{
			UaConfiguration::instance()->setDeviceName( value );
		}
		break;

		case CallWaitingOnTag:
		{
			char key[ VALUE_MAX_LENGTH ];
			char dest[ VALUE_MAX_LENGTH ];
			sscanf( value, "%s %s", key, dest );
			bool b = isEqualNoCase(string( dest ), "True" );
			UaConfiguration::instance()->setCallWaitingOn(key, b);
		}
		break;

		case CallIdOnTag:
		{
			UaConfiguration::instance()->setCallIdOn( isEqualNoCase( string( value ), "True" ) );
		}
		break;

		case CallIdTag:
		{
			UaConfiguration::instance()->setCallId( value );
		}
		break;

		case UaTimeoutTag:
		{
			int i;
			sscanf( value, "%d", &i );
			UaConfiguration::instance()->setUaTimeout(i);
		}
		break;

		case SubscribeOnTag:
		{
			UaConfiguration::instance()->setSubscribeOn( isEqualNoCase( string( value ), "True" ) );
		}
		break;

		case SubscribeToTag:
		{
			UaConfiguration::instance()->setSubscribeTo( value );
		}
		break;

		case SubscribeExpireTag:
		{
			int i;
			sscanf( value, "%d", &i );
			UaConfiguration::instance()->setSubscribeExpire( i );
		}
		break;

		//added by lijie
		case LocalExchangeTag:
		{
			UaConfiguration::instance()->setLocalExchange(value);
		}
		break;

		case LocalBranchTag:
		{
			UaConfiguration::instance()->setLocalBranch(value);
		}
		break;

		case LocalDistrictTag:
		{
			UaConfiguration::instance()->setLocalDistrict(value);
		}
		break;

		case LocalCountryTag:
		{
			UaConfiguration::instance()->setLocalCountry(value);
		}
		break;

		case LocalDeviceTag:
		{
			UaConfiguration::instance()->setLocalDevice(value);
		}
		break;

		/* added 2007.03.08 */
		case ConsolePhoneTag:
		{
			UaConfiguration::instance()->setConsolePhone(value);
		}
		break;
		/* added 2007.03.27, lizhijie */
		case GreetingFileTag:
		{
			UaConfiguration::instance()->setGreetingFile(value);
		}
		break;
		
		case BusyTransferTag:
		{
			char key[ VALUE_MAX_LENGTH ];
			char dest[ VALUE_MAX_LENGTH ];
			sscanf( value, "%s %s", key, dest );
			UaConfiguration::instance()->setBusyTransfer(key, dest);
		}
		break;
		
		case TimeOutTransferTag:
		{
			char key[ VALUE_MAX_LENGTH ];
			char dest[ VALUE_MAX_LENGTH ];
			sscanf( value, "%s %s", key, dest );
			UaConfiguration::instance()->setTimeOutTransfer(key, dest);
		}
		break;
		case AllTransferTag:
		{
			char key[ VALUE_MAX_LENGTH ];
			char dest[ VALUE_MAX_LENGTH ];
			sscanf( value, "%s %s", key, dest );
			UaConfiguration::instance()->setAllTransfer(key, dest);
		}
		break;
		case MaxDropTimeTag:
		{
			int time;
			sscanf(value, "%d", &time);
			UaConfiguration::instance()->setMaxDropTime(time);
		}
		break;
		case WatchDogTimeTag:
		{
			int time;
			sscanf(value, "%d", &time);
			UaConfiguration::instance()->setWatchDogTime(time);
		}
		break;
		case PstnCountTag:
		{
			int time;
			sscanf(value, "%d", &time);
			UaConfiguration::instance()->setPstnCount(time);
		}
		break;
		//added ends here
		
		case VmServerTag:
		{
			UaConfiguration::instance()->addVMServer(string(value));
		}
		break;

		case LogFileNameTag:
		{
			UaConfiguration::instance()->setLogFilename(string(value));
		}
		break;

		case NATAddressTag:
		{
			UaConfiguration::instance()->setNATAddress(string(value));
		}
		break;

		case VadTag:
		{
			UaConfiguration::instance()->setVad( isEqualNoCase( string( value ), "True" ));
		}
		break;

		case JitterBufferTag:
		{
			UaConfiguration::instance()->setJitterBuffer(atoi( value) );
		}
		break;

		default:
		{
			cpLog( LOG_WARNING, "Unknown Tag: %s", tag );
		}
	}    /* switch */
}    // UaConfiguration::parseCfgFileCallBack


void UaConfiguration::instance( const string cfgFile )
{
	if (objcounts > 0)
		return;
	
	try
	{
		uaConfiguration = new UaConfiguration;
	}
	catch (bad_alloc&)
	{
		cpLog(LOG_EMERG, "Can't Allocate any memory for UaConfiguration");
		throw UaConfigurationErr("memory allocate error", __FILE__, __LINE__);
	}

	if(cfgFile.length())
	{
		if( !parse3tuple( cfgFile.c_str(), parseCfgFileCallBack ) )
		{
			delete uaConfiguration;
			throw UaConfigurationErr("Can't praser configuration file", __FILE__, __LINE__);
		}
	}

#if 0	
	/*add by Tang biao 2005-4-4*/
	UaConfiguration::instance()->change_flag=0;   
	UaConfiguration::instance()->alter_flag=0;
	UaConfiguration::instance()->public_cfgFile=cfgFile; 
	/*addy by Tang biao 2005-4-4*/
#endif
	UaConfiguration::instance()->write_pid_tofile();
	/*addy by Tang biao 2005-4-7*/

	uaConfiguration->show();
}    // UaConfiguration::instance


void UaConfiguration::show()
{
	cpDebug(LOG_DEBUG, "SIP User Agent Configuration" );
	cpDebug(LOG_DEBUG, "--- Device ---" );
	cpDebug(LOG_DEBUG, "       Device Name : \"%s\"", getDeviceName().c_str() );
	cpDebug(LOG_DEBUG, "--- User ---" );
	cpDebug(LOG_DEBUG, "              Name : \"%s\"", getUserName().c_str() );
	cpDebug(LOG_DEBUG, "      Display Name : \"%s\"", getDisplayName().c_str() );
	cpDebug(LOG_DEBUG, "         Pass Word : \"%s\"", getPassword().c_str() );

	cpDebug(LOG_DEBUG, "--- SIP ---" );
	cpDebug(LOG_DEBUG, "              Port : \"%s\"", getLocalSipPort().c_str() );
	cpDebug(LOG_DEBUG, "         Transport : \"%s\"", getSipTransport().c_str() );
	cpDebug(LOG_DEBUG, "      Proxy Server : \"%s\"", getProxyServer().c_str() );
	cpDebug(LOG_DEBUG, "      Proxy Port : \"%s\"", getProxyPort().c_str() );

	cpDebug(LOG_DEBUG, "--- Registration ---" );
	cpDebug(LOG_DEBUG, "          Register : \"%s\"", (getRegisterOn() ? "True" : "False" ) );
	cpDebug(LOG_DEBUG, "              From : \"%s\"", getRegisterFrom().c_str() );
	cpDebug(LOG_DEBUG, "                To : \"%s\"", getRegisterTo().c_str() );
	cpDebug(LOG_DEBUG, "           Expires : %d s", getRegisterExpires() );
	cpDebug(LOG_DEBUG, "            Action : \"%s\"", getRegisterAction().c_str() );
	cpDebug(LOG_DEBUG, "      Register Server : \"%s\"", getRegisterServer().c_str() );
	cpDebug(LOG_DEBUG, "      Register Port : \"%s\"", getRegisterPort().c_str() );

	cpDebug(LOG_DEBUG, "--- Subscribe ---" );
	cpDebug(LOG_DEBUG, "         Subscribe : \"%s\"", (getSubscribeOn() ? "True" : "False" ) );
	cpDebug(LOG_DEBUG, "                To : \"%s\"", getSubscribeTo().c_str() );
	cpDebug(LOG_DEBUG, "           Expires : %d s", getSubscribeExpire() );

	cpDebug(LOG_DEBUG, "--- Dialing ---" );
	cpDebug(LOG_DEBUG, "      Dial Timeout : %d ms", getInitialDigitTimeout() );
	cpDebug(LOG_DEBUG, "Interdigit Timeout : %d ms", getInterDigitTimeout() );

	cpDebug(LOG_DEBUG, "--- RTP ---" );
	cpDebug(LOG_DEBUG, "          RTP Rate : %d ms", getNetworkRtpRate() );

	cpDebug(LOG_DEBUG, "--- Misc Features ---" );
	cpDebug(LOG_DEBUG, "  Provide Ringback : \"%s\"", getProvideRingback() ? "True" : "False" );

	string xfermode = "XferOff";
	if ( getXferMode() == XferOn )
		xfermode = "Transfer";
	else if ( getXferMode() == ConfOn )
		xfermode = "Conference";
	
	cpDebug(LOG_DEBUG, "         Xfer Mode : \"%s\"", xfermode.c_str() );
	cpDebug(LOG_DEBUG, "  Conference Server: \"%s\"", getConfServer().c_str() );

	CallWaitMapType callWait = getCallWaits();

	CallWaitMapType::iterator callWaitIter = callWait.begin();

	//added by lijie 2005-06-03	
	while ( callWaitIter != callWait.end() )
	{
		cpDebug(LOG_DEBUG, " Call Waiting: %s	\"%s\"", callWaitIter->first.c_str(), callWaitIter->second ? "True" : "False" );
		callWaitIter++;
	}
    //added ends here	

	cpDebug(LOG_DEBUG, "            Ua_Timeout: %d s", getUaTimeout() );
	cpDebug(LOG_DEBUG, "		Max_DropTime: %d s", getMaxDropTime());
	cpDebug(LOG_DEBUG, "		WatchDog_Time: %d s", getWatchDogTime());
	cpDebug(LOG_DEBUG, "		Pstn_Count: %d", getPstnCount());	
	cpDebug(LOG_DEBUG, "		Local_Country: %s", getLocalCountry().c_str());	
	cpDebug(LOG_DEBUG, "		Local_District: %s", getLocalDistrict().c_str());
	cpDebug(LOG_DEBUG, "		Local_Device: %s", getLocalDevice().c_str());	
	cpDebug(LOG_DEBUG, "		Local_Branch: %s", getLocalBranch().c_str());		
	cpDebug(LOG_DEBUG, "		Console_Phone Number: %s", getConsolePhone().c_str());

	cpDebug(LOG_DEBUG, "       Min RTP Port: %d", getMinRtpPort() );
	cpDebug(LOG_DEBUG, "       Max RTP Port: %d", getMaxRtpPort() );
	cpDebug(LOG_DEBUG, "                VAD: \"%s\"", getVad() ? "True" : "False" );
	cpDebug(LOG_DEBUG, "      Jitter Buffer: %d ms", getJitterBuffer() );

	cpDebug(LOG_DEBUG, "--- Dial Patterns ---" );
	DialingPlanListType dp = getDialPlans();
	DialingPlanListType::iterator dpIter = dp.begin();
	while ( dpIter != dp.end() )
	{
		cpDebug(LOG_DEBUG, "%18d : %s", (*dpIter)->getDialMethod(), (*dpIter)->getDialPattern().c_str() );
		dpIter++;
	}

	cpDebug(LOG_DEBUG, "--- Speed Dials ---" );
	SpeedDialMapType sd = getSpeedDials();
	SpeedDialMapType::iterator sdIter = sd.begin();
	while ( sdIter != sd.end() )
	{
		cpDebug(LOG_DEBUG, "%18s : %s", sdIter->first.c_str(), sdIter->second.c_str() );
		sdIter++;
	}

	/* adde by lizhijie, 2005.02.14 */
	cpDebug(LOG_DEBUG, "--- Local Dials ---" );
	LocalDialMapType localdials = getLocalDials();
	LocalDialMapType::iterator localIter = localdials.begin();
	while ( localIter != localdials.end() )
	{
		cpDebug(LOG_DEBUG, "%18s : %s", localIter->first.c_str(), localIter->second.c_str() );
		localIter++;
	}
	cpDebug(LOG_DEBUG, "--- Pstn Dials ---" );
	PstnDialMapType pstndials = getPstnDials();
	PstnDialMapType::iterator pstnIter = pstndials.begin();
	while ( pstnIter != pstndials.end() )
	{
		cpDebug(LOG_DEBUG, "%18s : %s", pstnIter->first.c_str(), pstnIter->second.c_str() );
		pstnIter++;
	}
	/* end of added */

	//added by lijie
	cpDebug(LOG_DEBUG, "         Local_Exchange: %s", getLocalExchange().c_str() );

	cpDebug(LOG_DEBUG, "--- Busy Transfers ---" );
	TransferMapType tranfers = getBusyTransfers();
	TransferMapType::iterator transferIter = tranfers.begin();
	while ( transferIter != tranfers.end() )
	{
		cpDebug(LOG_DEBUG, "%18s : %s", transferIter->first.c_str(), transferIter->second.c_str() );
		transferIter++;
	}

	cpDebug(LOG_DEBUG, "--- Time Out Transfers ---" );
	tranfers = getTimeOutTransfers();
	transferIter = tranfers.begin();
	while ( transferIter != tranfers.end() )
	{
		cpDebug(LOG_DEBUG, "%18s : %s", transferIter->first.c_str(), transferIter->second.c_str() );
		transferIter++;
	}

	cpDebug(LOG_DEBUG, "--- All Transfers ---" );
	tranfers = getAllTransfers();
	transferIter = tranfers.begin();
	while ( transferIter != tranfers.end() )
	{
		cpDebug(LOG_DEBUG, "%18s : %s", transferIter->first.c_str(), transferIter->second.c_str() );
		transferIter++;
	}
	//added ends here

	cpDebug(LOG_DEBUG, "--- Manual CallId ---" );
	cpDebug(LOG_DEBUG, "      Manual CallId: \"%s\"", (getCallIdOn() ? "True" : "False" ) );
	cpDebug(LOG_DEBUG, "             CallId: \"%s\"", getCallId().c_str() );

	cpDebug(LOG_DEBUG, "         Pbx_Count: %d", getPbxCount() );
	cpDebug(LOG_DEBUG, "         Log_Level: %d", getLogLevel() );
	cpDebug(LOG_DEBUG, "         G711_Law: %d", getG711Law() );
	cpDebug(LOG_DEBUG, "         Gain_RX: %f", getRxGain() );
	cpDebug(LOG_DEBUG, "         Gain_TX: %f", getTxGain() );
	cpDebug(LOG_DEBUG, "         Pbx_DTMF_Mode: %d", getPbxDtmfMode() );
	cpDebug(LOG_DEBUG, "         Prev_URL_Size: %d", getPrevUrlSize() );

	cpLog(LOG_WARNING, "      RxGain : %d ", UaConfiguration::instance()->getRxGain() );
	cpLog(LOG_WARNING, "      TxGain : %d ", UaConfiguration::instance()->getTxGain() );
	cpDebug(LOG_DEBUG, "\n\n");
	

}    // UaConfiguration::show

string UaConfiguration::getUserName()
{
	return userName;
}    // UaConfiguration::getUserName


string UaConfiguration::getDisplayName()
{
	return displayName;
}    // UaConfiguration::getDisplayName


string UaConfiguration::getPassword()
{
    return password;
}    // UaConfiguration::getPassword

string UaConfiguration::getLocalSipPort()
{
    return localSipPort;
}    // UaConfiguration::getLocalSipPort


int UaConfiguration::getMinRtpPort()
{
    return minRtpPort;
}
    
int UaConfiguration::getPstnCount() const
{
    return pstnCount;
}

void UaConfiguration::setPstnCount(int count)
{
    pstnCount = count;
}


int
UaConfiguration::getMaxRtpPort()
{
    return maxRtpPort;
}


string
UaConfiguration::getSipTransport()
{
    return sipTransport;
}    // UaConfiguration::getSipTransport


string UaConfiguration::getProxyServer()
{
    return proxyServer;
}    // UaConfiguration::getProxyServer


bool UaConfiguration::getRegisterOn()
{
    return registerOn;
}    // UaConfiguration::getRegisterOn


string
UaConfiguration::getRegisterFrom()
{
    return registerFrom;
}    // UaConfiguration::getRegisterFrom


string
UaConfiguration::getRegisterTo()
{
    return registerTo;
}    // UaConfiguration::getRegisterTo


int
UaConfiguration::getRegisterExpires()
{
    return registerExpires;
}    // UaConfiguration::getRegisterExpires


string
UaConfiguration::getRegisterAction()
{
    return registerAction;
}    // UaConfiguration::getRegisterAction


int
UaConfiguration::getNetworkRtpRate()
{
    return networkRtpRate;
}    // UaConfiguration::getNetworkRtpRate


bool
UaConfiguration::getProvideRingback()
{
    return provideRingback;
}    // UaConfiguration::getNetworkRtpRate


int
UaConfiguration::getInitialDigitTimeout()
{
    return initialDigitTimeout;
}    // UaConfiguration::getInitialDigitTimeout


int
UaConfiguration::getInterDigitTimeout()
{
    return interDigitTimeout;
}    // UaConfiguration::getInterDigitTimeout


DialingPlanListType&
UaConfiguration::getDialPlans()
{    
    return dialPlans;    
}    // UaConfiguration::getDialPlans


SpeedDialMapType&
UaConfiguration::getSpeedDials()
{    
    return speedDials;    
}    // UaConfiguration::getSpeedDials

/* added by lizhijie */
LocalDialMapType& UaConfiguration::getLocalDials()
{
       	return localDials;
}    

UaXferMode
UaConfiguration::getXferMode() const
{
    return xferMode;
}


string
UaConfiguration::getConfServer() const
{
    return confServer;
}

//added by lijie
string
UaConfiguration::getLocalExchange() const
{
    return localExchange;
}

void UaConfiguration::setLocalExchange(string _localExchange)
{
    localExchange = _localExchange;
}

TransferMapType& UaConfiguration::getTimeOutTransfers()
{
	return timeoutTransfer;
}
TransferMapType& UaConfiguration::getAllTransfers()
{
	return allTransfer;
}
TransferMapType& UaConfiguration::getBusyTransfers()
{
	return busyTransfer;
}

string UaConfiguration::getBusyTransfer(string key)
{
	TITER trIter = busyTransfer.find(key);
	TITER trItere = busyTransfer.end();
	if(trIter != trItere)
		return trIter->second;
	return "";
}
void UaConfiguration::setBusyTransfer(string key, string value )
{
	busyTransfer[key] = value;
}
string UaConfiguration::getTimeOutTransfer(string key)
{
	TITER trIter = timeoutTransfer.find(key);
	TITER trItere = timeoutTransfer.end();
	if(trIter != trItere)
		return trIter->second;
	return "";
}
void UaConfiguration::setTimeOutTransfer(string key, string value)
{
	timeoutTransfer[key] = value;
}

string UaConfiguration::getAllTransfer(string key)
{
	TITER trIter = allTransfer.find(key);
	TITER trItere = allTransfer.end();
	if(trIter != trItere)
		return trIter->second;
	return "";
}

void UaConfiguration::setAllTransfer(string key, string value)
{
	allTransfer[key] = value;
}

//added ends here

void
UaConfiguration::setUserName( string name )
{
    userName = name;
}    // UaConfiguration::setUserName


void
UaConfiguration::setDisplayName( string name )
{
    displayName = name;
}    // UaConfiguration::setDisplayName


void
UaConfiguration::setPassword( string pwd )
{
    password = pwd;
}    // UaConfiguration::setPassword


void UaConfiguration::setLocalSipPort( string port )
{
	localSipPort = port;
}    // UaConfiguration::setLocalSipPort

void UaConfiguration::setMinRtpPort( int port )
{
	minRtpPort = port;
}    // UaConfiguration::getMinRtpPort


void
UaConfiguration::setMaxRtpPort( int port )
{
    maxRtpPort = port;
}    // UaConfiguration::getMaxRtpPort


void
UaConfiguration::setSipTransport( string transport )
{
    sipTransport = transport;
}    // UaConfiguration::setSipTransport


void UaConfiguration::setProxyServer( string server )
{
	proxyServer = server;
}    // UaConfiguration::setProxyServer


void
UaConfiguration::setRegisterOn( bool on )
{
    registerOn = on;
}    // UaConfiguration::setRegisterOn


void
UaConfiguration::setRegisterFrom( string from )
{
    registerFrom = from;
}    // UaConfiguration::setRegisterFrom


void
UaConfiguration::setRegisterTo( string to )
{
    registerTo = to;
}    // UaConfiguration::setRegisterTo


void
UaConfiguration::setRegisterExpires( int expires )
{
    registerExpires = expires;
}    // UaConfiguration::setRegisterExpires


void
UaConfiguration::setRegisterAction( string action )
{
    registerAction = action;
}    // UaConfiguration::setRegisterAction


void
UaConfiguration::setNetworkRtpRate( int rate )
{
    networkRtpRate = rate;
}    // UaConfiguration::setNetworkRtpRate


void
UaConfiguration::setProvideRingback( bool ringback )
{
    provideRingback = ringback;
}    // UaConfiguration::setNetworkRtpRate


void
UaConfiguration::setInitialDigitTimeout( int timeout )
{
    initialDigitTimeout = timeout;
}    // UaConfiguration::setInitialDigitTimeout


void
UaConfiguration::setInterDigitTimeout( int timeout )
{
    interDigitTimeout = timeout;
}    // UaConfiguration::setInterDigitTimeout


void
UaConfiguration::addDialPattern( DialMethodsType type, string pattern )
{
    DialingPlan* plan = new DialingPlan( pattern, type );
    dialPlans.push_back( plan );
}    // UaConfiguration::addDialPattern


void
UaConfiguration::addSpeedDial( string key, string dest )
{
    speedDials[ key ] = dest;
}    // UaConfiguration::addSpeedDial

/* added by lizhijie */
void UaConfiguration::addLocalDial( string key, string dest )
{
	localDials[ key ] = dest;
} 

void UaConfiguration::addPstnDial( string key, string dest )
{
	pstnDials[ key ] = dest;
} 

PstnDialMapType& UaConfiguration::getPstnDials()
{
	return pstnDials;
}

/*added li zhijie 2007.05.12 */
void UaConfiguration::addPstnHwStatus( int index, bool isOn )
{
	pstnHwStatusMap[ index ] = isOn;
} 

PstnHwStatusMap& UaConfiguration::getPstnHwStatusMap()
{
	return pstnHwStatusMap;
}


/* added by wuaiwu, 2004/4/12 */

void UaConfiguration::addOthsDial(int type, const string & key, const string & dest)
{
	switch (type)
	{
	case 0:
		recallDials[key] = dest;
		break;
	case 1:
		spKeyDials[key] = dest;
		break;
	default:
		break;
	}
	
}

OthsDialMapType& UaConfiguration::getOthsDials(int type)
{
	if (type == 0)
	{
		return recallDials;
	}
	else
	{
		return spKeyDials;
	}
}

/* end of added */

const char* UaConfiguration::getPhoneNumberByDeviceName(char *devicename)
{
	//UaConfiguration::instance()->config_Mutex.lock();
	if( devicename !=0)
	{
		LocalDialMapType::iterator iter = localDials.begin();
		while ( iter != localDials.end() )
		{
			if( strcmp( devicename , iter->second.c_str() )==0 ) 
			{
				return iter->first.c_str();
			}
			iter++;
		}
		PstnDialMapType::iterator pstnIter = pstnDials.begin();
		while ( pstnIter != pstnDials.end() )
		{
			if( strcmp( devicename , pstnIter->second.c_str() )==0 ) 
			{
				return pstnIter->first.c_str();
			}
			pstnIter++;
		}
	}
	//UaConfiguration::instance()->config_Mutex.unlock();
	return 0;
}

int UaConfiguration::getAnswerRestartTimer()
{
	return answerRestartTimer;
}

void UaConfiguration::setAnswerRestartTimer(int delay)
{
	answerRestartTimer = delay;
}


bool
UaConfiguration::getRsvpOn() const
{
    return rsvpOn;
}


string
UaConfiguration::getProvisioningHost() const
{
    return provisioningHost;
}


int
UaConfiguration::getProvisioningPort() const
{
    return provisioningPort;
}


bool
UaConfiguration::getUsePolicyServer() const
{
    return usePolicyServer;
}


string
UaConfiguration::getDeviceName() const
{
    return deviceName;
}


bool
UaConfiguration::getCallWaitingOn(string phone)
{

 	map<string, bool>::iterator iter = callWait.find(phone);
	if(iter != callWait.end())
		return iter->second;
	return false;
}


bool
UaConfiguration::getCallIdOn()
{
    return myCallIdOn;
}


string
UaConfiguration::getCallId()
{
    return myCallId;
}


int
UaConfiguration::getUaTimeout() const
{
    return uaTimeout;
}


bool
UaConfiguration::getSubscribeOn() const
{
    return subscribeOn;
}    // UaConfiguration::getSubscribeOn


string
UaConfiguration::getSubscribeTo() const
{
    return subscribeTo;
}    // UaConfiguration::getSubscribeTo


int
UaConfiguration::getSubscribeExpire() const
{
    return subscribeExpire;
}    // UaConfiguration::getSubscribeExpire


void
UaConfiguration::setRsvpOn( const bool on )
{
    rsvpOn = on;
}


void
UaConfiguration::setProvisioningHost( const string host )
{
    provisioningHost = host;
}


void
UaConfiguration::setProvisioningPort( const int port )
{
    provisioningPort = port;
}


void
UaConfiguration::setUsePolicyServer( const bool use )
{
    usePolicyServer = use;
}


void
UaConfiguration::setXferMode( UaXferMode xfermode )
{
    xferMode = xfermode;
}


void
UaConfiguration::setConfServer( string confserver )
{
    confServer = confserver;
}

void UaConfiguration::setMaxDropTime(int time)
{
	if(time<15)
		time = 15;
	
	maxDropTime = time;
}

int UaConfiguration::getMaxDropTime() const
{
	return maxDropTime;
}

void UaConfiguration::setWatchDogTime(int time)
{
	watchDogTime = time;
}

int UaConfiguration::getWatchDogTime() const
{
	return watchDogTime;
}


void
UaConfiguration::setCallId( const string callid )
{
    if (callid.length() <= 0 )
    {
        myCallId = "";
    }
    else
    {
        myCallId = callid;
    }
}


void
UaConfiguration::setCallIdOn( const bool callIdOn )
{
    myCallIdOn = callIdOn;
}


void UaConfiguration::setDeviceName( const string devicename )
{
	if( strcmp(devicename.c_str(), "SLIC") == 0)
		appType = assist_app_slic;
	else if( strcmp(devicename.c_str(), "PCM") == 0)
		appType = assist_app_pcm;
	else
		assert(0);
	deviceName = devicename;
}


void
UaConfiguration::setCallWaitingOn( const string phone, const bool value )
{
  	callWait[phone] = value;
}

CallWaitMapType&
UaConfiguration::getCallWaits(  )
{
  	return callWait;
}


void
UaConfiguration::setUaTimeout(const int timeout)
{
    uaTimeout = timeout;
}


void
UaConfiguration::setSubscribeOn( const bool subOn )
{
    subscribeOn = subOn ;
}    // UaConfiguration::setSubscribeOn


void
UaConfiguration::setSubscribeTo( const string to )
{
    subscribeTo = to;
}    // UaConfiguration::setSubscribeTo


void
UaConfiguration::setSubscribeExpire( const int expire )
{
    subscribeExpire = expire;
}    // UaConfiguration::setSubscribeExpire


void
UaConfiguration::addVMServer(const string server)
{
    myVmServers.push_back(server);
}


vector<string> &
UaConfiguration::getVMServers()
{
    return myVmServers;
}


void
UaConfiguration::setLogFilename(const string& filename)
{
    myLogFilename = filename;
}


string
UaConfiguration::getLogFilename() const
{
    return myLogFilename;
}


void
UaConfiguration::setNATAddress(const string& address)
{
    myNATAddress = address;
}


string
UaConfiguration::getNATAddress() const
{
    return myNATAddress;
}


UaConfiguration::UaConfiguration()
{
        /*userName(""),
	  displayName(""),
        password("\0"),
        localSipPort("5060"),
        minRtpPort( 1024 ),
        maxRtpPort( 2<<16-1 ),
        sipTransport("UDP"),
        proxyServer("\0"),
        registerOn( false ),
        registerFrom(""),
        registerTo(""),
        registerExpires(60*60*60),
        registerAction("Proxy"),  
        networkRtpRate( 20 ),          // 20 ms
        provideRingback(false),
        initialDigitTimeout( 16000 ),  // 16 seconds
        interDigitTimeout( 8000 ),     // 8 seconds
        dialPlans(0),
        speedDials(0),
        localDials(0),
        pbxCount(1),
        logLevel(0),
        rxGain((float)0),
        txGain(float(0)),
 //       g711Law(as_g711_a_law),
        pbxDtmfMode(0),
        prevUrlSize(10),
        appType(assist_app_unknow),
        recallDials(0),
        spKeyDials(0),
        answerRestartTimer( 12000 ),
        localExchange(""),
        busyTransfer(0),
        timeoutTransfer(0),
        allTransfer(0),
        rsvpOn(false),
        provisioningHost(""),
        provisioningPort(0),
        usePolicyServer(false),
	 deviceName("/dev/astel"),
        xferMode(XferOff),
        confServer(""),
        myCallId(""),
        myCallIdOn( false ),
        callWait(0),
        uaTimeout( 5 ),
        subscribeOn( false),
        subscribeExpire( 0 ),
        subscribeTo( "" ),
        myVmServers(0),
        myLogFilename(""),
        myNATAddress(""),
        vadOn(false),
        jitterBuffer(0),
        watchDogTime(5),
        maxDropTime(15),
        registerServer(""),
        UserInfoMap(0),
       Authentication("Off")*/

	 userName = "";
	  displayName = "";
        password = "\0",
        localSipPort = "5056";
        minRtpPort = 1024 ;
        maxRtpPort =  2<<16-1;
        sipTransport = "UDP";
        proxyServer = "\0";
        registerOn = false;
        registerFrom = "";
        registerTo = "";
        registerExpires = 60*60*60;
        registerAction = "Proxy";
        networkRtpRate =  20 ;          // 20 ms
        provideRingback = false;
        initialDigitTimeout = 16000 ;  // 16 seconds
        interDigitTimeout =  8000 ;     // 8 seconds
    //    dialPlans = 0;
    //    speedDials =0;
    //    localDials =0;
        pbxCount = 1;
        logLevel = 0;
        rxGain = 0;
        txGain = 0;
        g711Law = as_g711_a_law;
        pbxDtmfMode = 0;
        prevUrlSize = 10;
        appType = assist_app_unknow;
   //     recallDials(0),
    //    spKeyDials(0),
        answerRestartTimer = 12000 ;
        localExchange = "";
  //      busyTransfer = 0,
   //     timeoutTransfer = 0,
   //     allTransfer = 0;
        rsvpOn = false;
        provisioningHost = "";
        provisioningPort = 0;
        usePolicyServer = false;
	 deviceName = "/dev/astel";
        xferMode = XferOff;
        confServer = "";
        myCallId = "";
        myCallIdOn = false;
       // callWait = 0;
        uaTimeout = 5 ;
        subscribeOn = false;
        subscribeExpire = 0;
        subscribeTo = "";
        //myVmServers = 0;
        myLogFilename = "";
        myNATAddress ="";
        vadOn =false;
        jitterBuffer =0;
	  cdrOn = 0;
	 fifoName = "";
	 cdrName = "";
        watchDogTime = 5;
        maxDropTime = 15;
#if 0		
	 localCountry = "86";
	 localDistrict = "28";
	 localDevice = "0000";
	 localBranch = "";
#else/* lzj ,2007.03.08 */
	 localCountry = "";
	 localDistrict = "";
	 localDevice = "";
	 localBranch = "";
#endif
        registerServer = "";
   //     UserInfoMap(0),
       Authentication = "Off";

	/* added 2007.03.09 */
	registerPort = "9060";
	proxyPort = "9060";
	
}    // UaConfiguration::UaConfiguration


UaConfiguration::~UaConfiguration()
{
}    // UaConfiguration::~UaConfiguration


void 
UaConfiguration::write_pid_tofile()
{
  	FILE *p;
  	char pid_string[1024];
  	char name_string[1024];
	
  	memset(pid_string,0,1024);
  	memset(name_string,0,1024);
  	p=fopen(IPPBX_PID_FILE, "w+t");
  	if(!p)
  	{
    		cpLog(LOG_ERR, "Open file %s failed, exit", IPPBX_PID_FILE);
    		throw UaConfigurationErr("Can't open pid file",
           					        __FILE__,
           					        __LINE__);
  	}
  	//sprintf(pid_string,"565");
  	sprintf(pid_string,"%d\n",getpid());
  	strcpy(name_string,"pbx\n");  
  	fputs(pid_string,p);
  	fputs(name_string,p);
  	fclose(p);
  // 
}


#if 0

/**add by tang biao  2005-4-4*/
void
UaConfiguration::flushSpeedDial()
{
    speedDials.clear();
}

void
UaConfiguration::flushLocalDial()
{
    localDials.clear();
}

void
UaConfiguration::flushDialPlan()
{
  dialPlans.clear();
}
void 
UaConfiguration::reload_config()
{
  UaConfiguration::instance()->change_flag=1;
  UaConfiguration::instance()->config_Mutex.lock();
  UaConfiguration::instance()->flushSpeedDial();
  UaConfiguration::instance()->flushLocalDial();
  UaConfiguration::instance()->flushDialPlan(); 
  if(public_cfgFile.length())
  {
     if( !parse3tuple( public_cfgFile.c_str(), parseCfgFileCallBack ) )
     {
            // Stop UA
	    cout<<"Dont't reload config file"<<endl;
            exit( -1 );
     }
  }
  UaConfiguration::instance()->config_Mutex.unlock();
  cpLogSetPriority(UaConfiguration::instance()->getLogLevel());
   if(UaConfiguration::instance()->getLogLevel()==LOG_DEBUG)
		UaConfiguration::instance()->show();

  UaConfiguration::instance()->change_flag=0;
  UaConfiguration::instance()->alter_flag=1;  
  int retval;
  retval=cpLogOpen(UaConfiguration::instance()->getLogFilename().c_str());

  //return 0;
}
int
UaConfiguration::getflag_change()
{
  return change_flag;
}
int
UaConfiguration::getflag_alter()
{
  return alter_flag;
}


void
UaConfiguration::resetflag_alter()
{
  alter_flag=0;
}

void
UaConfiguration::lock_config()
{
  config_Mutex.lock();
}
void
UaConfiguration::unlock_config()
{
  config_Mutex.unlock();
}
/**add by Tang biao  2005-4-4*/

/**add by Tang biao  2005-4-7*/
void
UaConfiguration::reload_parseCfgFileCallBack( char* tag, char* type, char* value )
{
    switch ( strConst2i( tag, UaConfigurationTagString, UaConfigurationTagMax ) )
    {
        case UserNameTag:
        {
            UaConfiguration::instance()->setUserName( value );
        }
        break;

        case DisplayNameTag:
        {
            UaConfiguration::instance()->setDisplayName( value );
        }
        break;

        case PasswordTag:
        {
            UaConfiguration::instance()->setPassword( value );
        }
        break;

        case LocalSipPortTag:
        {
            //UaConfiguration::instance()->setLocalSipPort( value );
        }
        break;

	case MinRtpPortTag:
	{
            int i;
            sscanf( value, "%d", &i );
           // UaConfiguration::instance()->setMinRtpPort( i );
	}
	break;

	case MaxRtpPortTag:
	{
            int i;
            sscanf( value, "%d", &i );
            //UaConfiguration::instance()->setMaxRtpPort( i );
	}
	break;

        case SipTransportTag:
        {
            //UaConfiguration::instance()->setSipTransport( value );
        }
        break;

        case ProxyServerTag:
        {
            UaConfiguration::instance()->setProxyServer( value );
        }
        break;

        case RegisterOnTag:
        {
            UaConfiguration::instance()->setRegisterOn( isEqualNoCase( string( value ), "True" ) );
        }
        break;

        case RegisterFromTag:
        {
            UaConfiguration::instance()->setRegisterFrom( value );
        }
        break;

        case RegisterToTag:
        {
            UaConfiguration::instance()->setRegisterTo( value );
        }
        break;

        case RegisterExpiresTag:
        {
            int i;
            sscanf( value, "%d", &i );
            UaConfiguration::instance()->setRegisterExpires( i );
        }
        break;

        case RegisterActionTag:
        {
            UaConfiguration::instance()->setRegisterAction( value );
        }
        break;

        case NetworkRtpRateTag:
        {
            int i;
            sscanf( value, "%d", &i );
            //UaConfiguration::instance()->setNetworkRtpRate( i );
        }
        break;

        case ProvideRingbackTag:
        {
            int flag;
            sscanf( value, "%d", &flag );
            //UaConfiguration::instance()->setProvideRingback( isEqualNoCase( string( value ), "True" ) ) ;
        }
        break;

        case InitialDigitTimeoutTag:
        {
            int i;
            sscanf( value, "%d", &i );
            UaConfiguration::instance()->setInitialDigitTimeout( i );
        }
        break;

        case InterDigitTimeoutTag:
        {
            int i;
            sscanf( value, "%d", &i );
            UaConfiguration::instance()->setInterDigitTimeout( i );
        }
        break;

        case DialPatternTag:
        {
            int type;
            char pattern[ VALUE_MAX_LENGTH ];
            sscanf( value, "%d %s", &type, pattern );
            UaConfiguration::instance()->addDialPattern( (DialMethodsType)type,
                    pattern );
        }
        break;

        case SpeedDialTag:
        {
            char key[ VALUE_MAX_LENGTH ];
            char dest[ VALUE_MAX_LENGTH ];
            sscanf( value, "%s %s", key, dest );
            UaConfiguration::instance()->addSpeedDial( key, dest );
        }
        break;

		/* added by lizhijie, 2005.02.14 */
		case LocalDialTag:
		{
			char key[ VALUE_MAX_LENGTH ];
			char dest[VALUE_MAX_LENGTH];
			char val[VALUE_MAX_LENGTH];
			sscanf( value, "%s %s", key, dest );

			sprintf(val, "%s_%s", UaConfiguration::instance()->getDeviceName().c_str(), dest );
			UaConfiguration::instance()->addLocalDial( key, val );
		}
		break;
		
		/* added by lizhijie, 2005.02.19 */
		case PbxCountTag:
		{
			int i;
			sscanf( value, "%d", &i );
			//UaConfiguration::instance()->setPbxCount( i );
		}
		break;

		/* added by lizhijie, 2005.02.22 */
		case LogLevelTag:
		{
			int i;
			sscanf( value, "%d", &i );
			UaConfiguration::instance()->setLogLevel( i );
		}
		break;

 		/* added by lizhijie, 2005.02.23 */
		case RxGainTag:
		{
			float f;
			sscanf( value, "%f", &f );
			//UaConfiguration::instance()->setRxGain( f );
		}
		break;
		case TxGainTag:
		{
			float f;
			sscanf( value, "%f", &f );
			//UaConfiguration::instance()->setTxGain( f );
		}
		break;
		case G711LawTag:
		{
			int i;
			sscanf( value, "%d", &i );
			//UaConfiguration::instance()->setG711Law( (as_law_type_t) i );
		}
		break;
		case PbxDtmfModeTag:
		{
			int i;
			sscanf( value, "%d", &i );
			//UaConfiguration::instance()->setPbxDtmfMode( i );
		}
		break;
		case PrevUrlSizeTag:
		{
			int i;
			sscanf( value, "%d", &i );
			//UaConfiguration::instance()->setPrevUrlSize( i );
		}
		break;
		/* end of added by lizhijie 02.23 */

		case AnswerRestartTimerTag:
		{
			int i;
			sscanf( value, "%d", &i );
		//	UaConfiguration::instance()->setAnswerRestartTimer( i );
		}
		break;

        case RsvpOnTag:
        {
           // UaConfiguration::instance()->setRsvpOn( isEqualNoCase(string( value ), "True" ) );
        }
        break;

        case ProvisioningHostTag:
        {
           // UaConfiguration::instance()->setProvisioningHost( value );
        }
        break;

        case ProvisioningPortTag:
        {
            int i;
            sscanf( value, "%d", &i );
            //UaConfiguration::instance()->setProvisioningPort( i );
        }
        break;

        case UsePolicyServerTag:
        {
            //UaConfiguration::instance()->setUsePolicyServer( isEqualNoCase(string( value ), "True" ) );
        }
        break;

        case UaXferModeTag:
        {
            UaXferMode xfermode = XferOff;
            if ( string( value ) == "Transfer" )
	    {
                xfermode = XferOn;
	    }
            else if ( string( value ) == "Conference" )
	    {
                xfermode = ConfOn;
	    }
            //UaConfiguration::instance()->setXferMode( xfermode );
        }
        break;

        case ConfServerTag:
        {
            //UaConfiguration::instance()->setConfServer( string(value) );
        }
        break;

        case DeviceNameTag:
        {
            //UaConfiguration::instance()->setDeviceName( value );
        }
        break;

        case CallWaitingOnTag:
        {
            //UaConfiguration::instance()->setCallWaitingOn( isEqualNoCase(string( value ), "True" ) );
        }
        break;

        case CallIdOnTag:
        {
           // UaConfiguration::instance()->setCallIdOn( isEqualNoCase( string( value ), "True" ) );
        }
        break;

        case CallIdTag:
        {
           // UaConfiguration::instance()->setCallId( value );
        }
        break;

        case UaTimeoutTag:
        {
            int i;
            sscanf( value, "%d", &i );
           // UaConfiguration::instance()->setUaTimeout(i);
        }
        break;

        case SubscribeOnTag:
        {
           // UaConfiguration::instance()->setSubscribeOn( isEqualNoCase( string( value ), "True" ) );
        }
        break;

        case SubscribeToTag:
        {
           // UaConfiguration::instance()->setSubscribeTo( value );
        }
        break;

        case SubscribeExpireTag:
        {
            int i;
            sscanf( value, "%d", &i );
            //UaConfiguration::instance()->setSubscribeExpire( i );
        }
        break;

    case VmServerTag:
    {
	//UaConfiguration::instance()->addVMServer(string(value));
    }
    break;

    case LogFileNameTag:
    {
	UaConfiguration::instance()->setLogFilename(string(value));
    }
    break;

    case NATAddressTag:
    {
	//UaConfiguration::instance()->setNATAddress(string(value));
    }
    break;

        default:
        {
            cpLog( LOG_WARNING, "Unknown Tag: %s", tag );
        }
    }    /* switch */
}    // UaConfiguration::parseCfgFileCallBack
/*add by Tangbiao 2005-4-11*/
#endif
bool UaConfiguration::getVad()
{
	return vadOn;
}

void UaConfiguration::setVad(bool flag)
{
	vadOn=flag;
}

int UaConfiguration::getJitterBuffer()
{
	return jitterBuffer;
}

void UaConfiguration::setJitterBuffer(int jitterNew)
{
	jitterBuffer=jitterNew;
}

/*added by wuaiwu 2005/06/29 */
void UaConfiguration::setRegisterServer(const char * tag)
{
	registerServer = tag;
}

const string& UaConfiguration::getRegisterServer() const
{
 	return registerServer;
}
/* end of added */

/*added by lizhijie 2007/03/09 */
void UaConfiguration::setRegisterPort(string _port)
{
	registerPort = _port;
}
const string& UaConfiguration::getRegisterPort() const
{
 	return registerPort;
}

void UaConfiguration::setProxyPort(string _port)
{
	proxyPort = _port;
}
const string& UaConfiguration::getProxyPort() const
{
 	return proxyPort;
}

/* end of added */

assist_app_type  UaConfiguration::getAppType() const
{
	return appType;
}

UaConfiguration* UaConfiguration::LocalInit()
{
	UaConfiguration::instance(IPPBX_CONFIG_FILE);
	return uaConfiguration;
}

/* added lizhijie,2007.03.20 */
void UaConfiguration::setLogSize(int size)
{
	if(size>= 2048*1024 && size<= 0)
		logSize = 2048*1024;	
	else
		logSize = size;
}

int UaConfiguration::getLogSize()
{
	return logSize;
}


