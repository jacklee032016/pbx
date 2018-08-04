/*
* $Id: PbxConfiguration.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"
#include <cassert>
#include <cstdio>

#include "cpLog.h"
#include "parse3tuple.h"

#include "Data.hxx"
#include "EndPoint.hxx"

#include "PbxConfiguration.hxx"

using namespace Assist;

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



PbxConfiguration* PbxConfiguration::isdnConfiguration = 0;

PbxConfiguration* PbxConfiguration::instance()
{
	assert( isdnConfiguration != 0 );
	return isdnConfiguration;
}


typedef enum
{
	UserNameTag = 0,
	DisplayNameTag,
	PasswordTag,

	InitialDigitTimeoutTag,
	InterDigitTimeoutTag,

	DialPatternTag,
	SpeedDialTag,
	LocalDialTag ,

	CallReturnDialTag,    
	SpecialKeyDialTag, 
	
	PbxCountTag ,
	LogLevelTag ,
	RxGainTag,
	TxGainTag,
	PbxDtmfModeTag,
	PrevUrlSizeTag,

	AnswerRestartTimerTag,

	DeviceNameTag,
	CallWaitingOnTag,
	CallIdOnTag,
	CallIdTag,

	PstnStandardTag,
	
	CarTimesTag,
	CarCountTag,

	IsdnTimeoutTag,
	
	LogFileNameTag,
	GreetingFileNameTag,

	PowerOnCallTag,

	AccessLengthTag,
	AccessCodeTag,

	PlugInTag,

	ExtLengthTag,
	ExtCodeTag,
	ExtTag,

	IsdnLocalDialTag,
	IsdnExternalDialTag,

	ConsoleCallingTag,
	ConsoleAlertingTag,

	DtmfMarkTimeTag,
	DtmfDurationTimeTag,
	
	FxoDialLineModeTag,
	FxoDialModeTag,
	PstnJumpListTag,
	
	SipportTag,	
	SipTransportTag,	
	RegisterOnTag,	
	RegisterServerTag,	
	RegisterExpiresTag,	
	ProxyServerTag,	
	RegisterActionTag,	
	MinRTPPortTag,	
	MaxRTPPortTag,	
	VadStringTag,	
	JitterBufferTag,	
	StartCdrServerTag,	
	AuthenticationTag,	
	SubscribeOnTag,	
	SubscribeExpiresTag,	
	SubscribeToTag,	
	CdrPathTag,	
	FifoTag,	
	G711LawTag,	
	RsvpOnTag,	
	RegisterFromTag,      
	RegisterToTag,      
	NATAddressTag,    
	NetworkRTPRateTag,      
	UaXferModeTag,      
	ConfServerTag, 
	ProvideRingbackTag, 
	UaTimeoutTag,   
	UsePolicyServerTag,     
	LocalExchangeTag,      
	LocalCountryTag,  
	LocalDistrictTag,     
	LocalDeviceTag,
	ProvisioningHostTag,
	ProvisioningPortTag,
	UserInfoTag,
	LocalBranchTag,
	
	PbxConfigurationTagMax
} PbxConfigurationTags;


static const char* PbxConfigurationTagString [] =
{
	"User_Name",
	"Display_Name",
	"Pass_Word",

	"Initial_Digit_Timeout",
	"Inter_Digit_Timeout",
	
	"Dial_Pattern",
	"Speed_Dial",        
	"Local_Dial" , 

	"Call_Return", 
	"Special_Key", 
	
	"Pbx_Count" ,
	"Log_Level" ,
	"Gain_RX",
	"Gain_TX",
	"Pbx_DTMF_Mode",
	"Prev_URL_Size",
		
	"AnswerRestartTimer",

	"Device_Name",
	"Call_Waiting_On",
	"CallId_On",
	"CallId",
	
	"PSTN_Standard",

	"CAR_Times",
	"CAR_Count",
	
	"Isdn_Timeout",
	
	"LogFilename",
	"Greeting_File",

	"PowerOnCall",

	"Access_Length",
	"Access_Code",

	"PlugIn",

	"EXT_Length",
	"EXT_Code",
	"EXT",

	"ISDN_LOCAL_CODE",
	"ISDN_EXTENAL_CODE",
	
	"Console_Calling_Timeout",
	"Console_Alerting_Timeout",

	"DTMF_Mark_Time",
	"DTMF_Total_Time",
	
	"PSTN_Line_Mode",
	"PSTN_Dial_Mode",
	"PSTN_JUMP",

	"Local_SIP_Port",
	"SIP_Transport",	
	"Register_On",
	"Register_Server",	
	"Register_Expires",	
	"Proxy_Server",	
	"Register_Action",	
	"Min_RTP_Port",	
	"Max_RTP_Port",	
	"VadString",
	"JitterBuffer",
	"Start_CdrServer",
	"Authentication",	
	"Subscribe_On",	
	"Subscribe_Expires",	
	"Subscribe_To",
	"Cdr_Path",	
	"Fifo",	
	"G711_Law",
	"Rsvp_On",	
	"Register_From",      
	"Register_To",     
	"NATAddress",   
	"Network_RTP_Rate",  
	"Ua_Xfer_Mode",    
	"Conference_Server",  
	"Provide_Ringback",    
	"Ua_Timeout",  
	"Use_Policy_Server",   
	"Local_Exchange",     
	"Local_Country",     
	"Local_District",   
	"Local_Device",
	"Provisioning_Host",
	"Provisioning_Port",
	"UserInfo",
	"Local_Branch",

        "Unknown"
};


void PbxConfiguration::parseCfgFileCallBack( char* tag, char* type, char* value )
{
	switch ( strConst2i( tag, PbxConfigurationTagString, PbxConfigurationTagMax ) )
	{
		case UserNameTag:
		{
			PbxConfiguration::instance()->setUserName( value );
			break;
		}

		case DisplayNameTag:
		{
			PbxConfiguration::instance()->setDisplayName( value );
			break;
		}

		case PasswordTag:
		{
			PbxConfiguration::instance()->setPassword( value );
			break;
		}

		case InitialDigitTimeoutTag:
		{
			int i;
			sscanf( value, "%d", &i );
			PbxConfiguration::instance()->setInitialDigitTimeout( i );
			break;
		}

		case InterDigitTimeoutTag:
		{
			int i;
			sscanf( value, "%d", &i );
			PbxConfiguration::instance()->setInterDigitTimeout( i );
			break;
		}

		case DialPatternTag:
		{
			int type;
			char pattern[ VALUE_MAX_LENGTH ];
			sscanf( value, "%d %s", &type, pattern );
			PbxConfiguration::instance()->addDialPattern( (DialMethodsType)type, pattern );
			break;
		}

		case SpeedDialTag:
		{
			char key[ VALUE_MAX_LENGTH ];
			char dest[ VALUE_MAX_LENGTH ];
			sscanf( value, "%s %s", key, dest );
			PbxConfiguration::instance()->addSpeedDial( key, dest );
			break;
		}

		/* added by lizhijie, 2005.02.14 */
		case LocalDialTag:
		{
			char key[ VALUE_MAX_LENGTH ];
			char dest[VALUE_MAX_LENGTH];
			char val[VALUE_MAX_LENGTH];
			sscanf( value, "%s %s", key, dest );

			sprintf(val, "%s_%s", PbxConfiguration::instance()->getDeviceName().c_str(), dest );
			PbxConfiguration::instance()->addLocalDial( key, val );
			break;
		}

		case CallReturnDialTag:
		{
			char key[ VALUE_MAX_LENGTH ];
            		char dest[ VALUE_MAX_LENGTH ];

            		sscanf( value, "%s %s", key, dest );
//            		PbxConfiguration::instance()->addOthsDial( 0, key, dest );
			break;
		}

		case SpecialKeyDialTag:
		{
			char key[ VALUE_MAX_LENGTH ];
            		char dest[ VALUE_MAX_LENGTH ];

            		sscanf( value, "%s %s", key, dest );
 //           		PbxConfiguration::instance()->addOthsDial( 1, key, dest );
			break;
		}
		
		case PbxCountTag:
		{
			int i;
			sscanf( value, "%d", &i );
			PbxConfiguration::instance()->setPbxCount( i );
			break;
		}

		case LogLevelTag:
		{
			int i;
			sscanf( value, "%d", &i );
			PbxConfiguration::instance()->setLogLevel( i );
			break;
		}

		case RxGainTag:
		{
			float f;
			sscanf( value, "%f", &f );
			PbxConfiguration::instance()->setRxGain( f );
			break;
		}

		case TxGainTag:
		{
			float f;
			sscanf( value, "%f", &f );
			PbxConfiguration::instance()->setTxGain( f );
			break;
		}

		case PbxDtmfModeTag:
		{
			int i;
			sscanf( value, "%d", &i );
			PbxConfiguration::instance()->setPbxDtmfMode( i );
			break;
		}

		case PrevUrlSizeTag:
		{
			int i;
			sscanf( value, "%d", &i );
			PbxConfiguration::instance()->setPrevUrlSize( i );
			break;
		}

		case AnswerRestartTimerTag:
		{
			int i;
			sscanf( value, "%d", &i );
			PbxConfiguration::instance()->setAnswerRestartTimer( i );
			break;
		}

		case DeviceNameTag:
		{
			PbxConfiguration::instance()->setDeviceName( value );
			break;
		}

		case CallWaitingOnTag:
		{
			char key[ VALUE_MAX_LENGTH ];
			char dest[ VALUE_MAX_LENGTH ];
			sscanf( value, "%s %s", key, dest );
//			bool b = isEqualNoCase(string( dest ), "True" );
//			PbxConfiguration::instance()->setCallWaitingOn(key, b);
			break;
		}

		case PstnStandardTag:
		{
			char st[VALUE_MAX_LENGTH];
			sscanf( value, "%s", st );
			pstn_standard_t s = PSTN_STANDARD_JAPAN;
			if(strcasecmp(st, "Japan") != 0)
				s = PSTN_STANDARD_CHINA;

			PbxConfiguration::instance()->setPstnStandard( s);
			break;
		}

		case CallIdOnTag:
		{
			PbxConfiguration::instance()->setCallIdOn( isEqualNoCase( string( value ), "True" ) );
			break;
		}

		case CallIdTag:
		{
			PbxConfiguration::instance()->setCallId( value );
			break;
		}

		case CarTimesTag:
		{
			int i;
			sscanf(value, "%d", &i);
			PbxConfiguration::instance()->setCarTimes(i);
			break;
		}
		case CarCountTag:
		{
			int i;
			sscanf(value, "%d", &i);
			PbxConfiguration::instance()->setCarCount(i);
			break;
		}
		
		case IsdnTimeoutTag:
		{
			int i;
			sscanf( value, "%d", &i );
			PbxConfiguration::instance()->setIsdnTimeout(i);
			break;
		}

		case LogFileNameTag:
		{
			PbxConfiguration::instance()->setLogFilename(string(value));
			break;
		}
		case GreetingFileNameTag:
		{
			PbxConfiguration::instance()->setGreetingFileName(string(value));
			break;
		}

		case PowerOnCallTag:
		{
			int i;
			sscanf( value, "%d", &i );
			PbxConfiguration::instance()->setPowerOnCall( i );
			break;
		}

		case AccessLengthTag:
		{
			int i;
			sscanf( value, "%d", &i );
			PbxConfiguration::instance()->setAccessLength( i );
			break;
		}
		
		case PlugInTag:
		{
			char name[ VALUE_MAX_LENGTH ];
			char lib[VALUE_MAX_LENGTH];
			sscanf( value, "%s %s", name, lib );

			PbxConfiguration::instance()->addPlugIn( name, lib );
			break;
		}

		case AccessCodeTag:
		{
			char type[ VALUE_MAX_LENGTH ];
			char code[VALUE_MAX_LENGTH];
			sscanf( value, "%s %s", type, code );

			PbxConfiguration::instance()->setAccessCode( type, code );
			break;
		}

		case ExtLengthTag:
		{
			int i;
			sscanf( value, "%d", &i );
			PbxConfiguration::instance()->setExtLength( i);
			break;
		}
		
		case ExtCodeTag:
		{
			char code[VALUE_MAX_LENGTH];
			sscanf( value, "%s", code );

			PbxConfiguration::instance()->setExtAccessCode( code );
			break;
		}

		case ExtTag:
		{
			char access[ VALUE_MAX_LENGTH ];
			char number[VALUE_MAX_LENGTH];
			sscanf( value, "%s %s", access, number );

			PbxConfiguration::instance()->addExtPhoneNumber( access, number );
			break;
		}

		case IsdnLocalDialTag:
		{
			char code[VALUE_MAX_LENGTH];
			sscanf( value, "%s", code );
			PbxConfiguration::instance()->setIsdnLocalDialCode( code );
			break;
		}
		case IsdnExternalDialTag:
		{
			char code[VALUE_MAX_LENGTH];
			sscanf( value, "%s", code );
			PbxConfiguration::instance()->setIsdnExternalDialCode( code );
			break;
		}

		case ConsoleCallingTag:
		{
			int i;
			sscanf( value, "%d", &i );
			PbxConfiguration::instance()->setConsoleCallingTimeout( i );
			break;
		}
		case ConsoleAlertingTag:
		{
			int i;
			sscanf( value, "%d", &i );
			PbxConfiguration::instance()->setConsoleAlertingTimeout( i );
			break;
		}
		
		case FxoDialLineModeTag:
		{
			FxoLineMode mode = FXO_LINE_INDIRECT;
			char sMode[VALUE_MAX_LENGTH];
			sscanf( value, "%s", sMode );
			if( strcasecmp(sMode, "Direct" )==0 )
				mode = FXO_LINE_DIRECT;

			PbxConfiguration::instance()->setFxoLineMode( mode);
			break;
		}
		case FxoDialModeTag:
		{
			FxoDialMode mode = FXO_DIAL_PUSHBUTTON;
			char sMode[VALUE_MAX_LENGTH];
			sscanf( value, "%s", sMode );
			if( strcasecmp(sMode, "DP10" )==0 )
				mode = FXO_DIAL_DP10;
			else if(strcasecmp(sMode, "DP20")== 0)
				mode = FXO_DIAL_DP20;

			PbxConfiguration::instance()->setFxoDialMode( mode);
			break;
		}

		case DtmfMarkTimeTag:
		{
			int i;
			sscanf( value, "%d", &i );
			PbxConfiguration::instance()->setDtmfMarkTime( i );
			break;
		}
		case DtmfDurationTimeTag:
		{
			int i;
			sscanf( value, "%d", &i );
			PbxConfiguration::instance()->setDtmfDurationTime( i );
			break;
		}

		case PstnJumpListTag:
		{
			char number[VALUE_MAX_LENGTH];
			sscanf( value, "%s %s", number );

			PbxConfiguration::instance()->addPstnJumpNumber( number );
			break;
		}
		case SipportTag:
		{
			PbxConfiguration::instance()->setLocalSipPort( value );
			break;
		}
		case SipTransportTag:
		{
			PbxConfiguration::instance()->setSipTransport( value );
			break;
		}
		case RegisterOnTag:
		{
			PbxConfiguration::instance()->setRegisterOn( isEqualNoCase( string( value ), "True" ) );
			break;
		}
		case RegisterServerTag:
		{
			PbxConfiguration::instance()->setRegisterServer( value );
			break;
		}
		case RegisterExpiresTag:
		{
			int i;
			sscanf(value ,"%d", &i);
			PbxConfiguration::instance()->setRegisterExpires(i);
			break;
		}
		case ProxyServerTag:
		{
			PbxConfiguration::instance()->setProxyServer( value );
			break;
		}
		case RegisterActionTag:
		{
			PbxConfiguration::instance()->setRegisterAction( value );
			break;
		}
		case MinRTPPortTag:
		{
			int i;
			sscanf(value, "%d", &i);
			PbxConfiguration::instance()->setMinRTPPort(i);
			break;
		}
		case MaxRTPPortTag:
		{
			int i;
			sscanf(value, "%d", &i);
			PbxConfiguration::instance()->setMaxRTPPort(i);
			break;
		}
		case VadStringTag:
		{
			PbxConfiguration::instance()->setVadString( isEqualNoCase( string( value ), "True" ) );
			break;
		}
		case JitterBufferTag:
		{
			int i;
			sscanf(value, "%d", &i);
			PbxConfiguration::instance()->setJitterBuffer(i);
			break;
		}
		case StartCdrServerTag:
		{
			int i;
			sscanf(value,"%d", &i);
			PbxConfiguration::instance()->setStartCdrServer(i);
			break;
		}
		case AuthenticationTag:
		{
			PbxConfiguration::instance()->setAuthentication( isEqualNoCase( string( value ), "Local" ) );
			break;
		}
		case SubscribeOnTag:
		{
			PbxConfiguration::instance()->setSubscribeOn(isEqualNoCase( string( value ), "True" ));
			break;
		}
		case SubscribeExpiresTag:
		{
			int i;
			sscanf(value,"%d",&i);
			PbxConfiguration::instance()->setSubscribeExpires(i);
			break;
		}
		case SubscribeToTag:
		{
			PbxConfiguration::instance()->setSubscribeTo( value );
			break;
		}
		case CdrPathTag:
		{
			PbxConfiguration::instance()->setCdrPath( value ); 
			break;
		}
		case FifoTag:
		{
			PbxConfiguration::instance()->setFifo( value );
			break;
		}
		case G711LawTag:
		{
			int i;
			sscanf(value,"%d",&i);
			PbxConfiguration::instance()->setG711Law( i );
			break;
		}
		case RsvpOnTag:
		{
			PbxConfiguration::instance()->setRsvpOn(isEqualNoCase( string( value ), "True" ));
			break;
		}
		case RegisterFromTag:
		{
			PbxConfiguration::instance()->setRegisterFrom(value);
			break;
		}
		case RegisterToTag:
		{
			PbxConfiguration::instance()->setRegisterTo(value);
			break;
		}
       	case NATAddressTag:
		{
			PbxConfiguration::instance()->setNATAddress(string(value));
			break;
		}
		case NetworkRTPRateTag:
		{
			int i;
			sscanf(value,"%d",&i);
			PbxConfiguration::instance()->setNetworkRtpRate(i); 
			break;
		}
		case  UaXferModeTag:
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
            		PbxConfiguration::instance()->setXferMode( xfermode );
			break;
		}
		case ConfServerTag:
       	{
           		 PbxConfiguration::instance()->setConfServer( string(value) );
			  break;
        	}
		case ProvideRingbackTag:
        	{
            		int flag;
            		sscanf( value, "%d", &flag );
           		 PbxConfiguration::instance()->setProvideRingback( isEqualNoCase( string( value ), "True" ) ) ;
			  break;
		}
		case UaTimeoutTag:
        	{
            		int t;
            		sscanf( value, "%d", &t );
            		PbxConfiguration::instance()->setUaTimeout(t) ;
			break;
        	}
		 case UsePolicyServerTag:
        	{
            		PbxConfiguration::instance()->setUsePolicyServer( isEqualNoCase(string( value ), "True" ) );
			break;
		 }
		 case LocalExchangeTag:
		{	
			PbxConfiguration::instance()->setLocalExchange(value);
			break;
		 }
		 case LocalCountryTag:
		{
			PbxConfiguration::instance()->setLocalCountry(value);
			break;
		}
		 case LocalDistrictTag:
		 {
			PbxConfiguration::instance()->setLocalDistrict(value);
			break;
		 }
		 case LocalDeviceTag:
		 {
			PbxConfiguration::instance()->setLocalDevice(value);
			break;
		 }
		 case ProvisioningHostTag:
		 {
			PbxConfiguration::instance()->setProvisionHost( value );
			break;
		 }
		 case ProvisioningPortTag:
		 {
			int i;
			sscanf(value,"%d",&i);
			PbxConfiguration::instance()->setProvisionPort(i);
			break;
		 }
		 case UserInfoTag:
		{
			char key[ VALUE_MAX_LENGTH ];
            		char dest[ VALUE_MAX_LENGTH ];
            		sscanf( value, "%s %s", key, dest );
			PbxConfiguration::instance()->addUserInfo( key, dest );
			break;
		}
		 case LocalBranchTag:
		{
			PbxConfiguration::instance()->setLocalBranch(value);
			break;
		 }
		default:
		{
			cpLog( LOG_WARNING, "Unknown Tag: %s", tag );
		}
	}

}


void PbxConfiguration::instance( const string cfgFile )
{
	assert( isdnConfiguration == 0 );
	isdnConfiguration = new PbxConfiguration;
	assert( isdnConfiguration != 0 );

	if(cfgFile.length())
	{
		if( !parse3tuple( cfgFile.c_str(), parseCfgFileCallBack ) )
		{
			exit( -1 );
		}
	}
    
	PbxConfiguration::instance()->write_pid_tofile();

	isdnConfiguration->show();
}    // PbxConfiguration::instance


void PbxConfiguration::show()
{
	dspConfig.law = U_LAW_CODE;
	dspConfig.toneLength = 20;
	dspConfig.standard = pstnStandard;
	dspConfig.dtmfMarkTime = dtmfMarkTime;
	dspConfig.dtmfDuration = dtmfDurationTime;
	
	cpLog( LOG_DEBUG, "Assist VPBX Configuration" );
	cpLog( LOG_DEBUG, "--- Device ---" );
	cpLog( LOG_DEBUG, "       Device Name : \"%s\"", getDeviceName().c_str() );
	cpLog( LOG_DEBUG, "--- User ---" );
	cpLog( LOG_DEBUG, "              Name : \"%s\"", getUserName().c_str() );
	cpLog( LOG_DEBUG, "      Display Name : \"%s\"", getDisplayName().c_str() );
	cpLog( LOG_DEBUG, "         Pass Word : \"%s\"\n", getPassword().c_str() );

	cpLog( LOG_DEBUG, "--- Files ---" );
	cpLog( LOG_DEBUG, "      Log File \t: %s", getLogFilename().c_str() );
	cpLog( LOG_DEBUG, "      Greeting File \t: %s", getGreetingFileName().c_str() );

	cpLog( LOG_DEBUG, "--- Dialing ---" );
	cpLog( LOG_DEBUG, "      Dial Timeout : %d ms", getInitialDigitTimeout() );
	cpLog( LOG_DEBUG, "Interdigit Timeout : %d ms", getInterDigitTimeout() );
	cpLog( LOG_DEBUG, "           Timeout : %d s", getIsdnTimeout() );

	cpLog( LOG_DEBUG, "--- Dial Patterns ---" );
	DialingPlanListType dp = getDialPlans();
	DialingPlanListType::iterator dpIter = dp.begin();
	while ( dpIter != dp.end() )
	{
		cpLog( LOG_DEBUG, "%18d : %s", (*dpIter)->getDialMethod(), (*dpIter)->getDialPattern().c_str() );
		dpIter++;
	}

	cpLog( LOG_DEBUG, "--- Speed Dials ---" );
	SpeedDialMapType sd = getSpeedDials();
	SpeedDialMapType::iterator sdIter = sd.begin();
	while ( sdIter != sd.end() )
	{
		cpLog( LOG_DEBUG, "%18s : %s", sdIter->first.c_str(), sdIter->second.c_str() );
		sdIter++;
	}

	cpLog( LOG_DEBUG, "--- Local Dials ---" );
	LocalDialMapType localdials = getLocalDials();
	LocalDialMapType::iterator localIter = localdials.begin();
	while ( localIter != localdials.end() )
	{
		cpLog( LOG_DEBUG, "%18s : %s", localIter->first.c_str(), localIter->second.c_str() );
		localIter++;
	}

	cpDebug( LOG_DEBUG, "--- Manual CallId ---" );
	cpDebug( LOG_DEBUG, "      Manual CallId: \"%s\"", (isCallIdOn() ? "ON" : "OFF" ) );
	cpDebug( LOG_DEBUG, "             CallId: \"%s\"", getCallId().c_str() );
	
	cpDebug( LOG_DEBUG, "         Pbx_Count: %d", getPbxCount() );
	cpDebug( LOG_DEBUG, "         Log_Level: %d", getLogLevel() );
	cpDebug( LOG_DEBUG, "         Gain_RX: %f", getRxGain() );
	cpDebug( LOG_DEBUG, "         Gain_TX: %f", getTxGain() );
	cpDebug( LOG_DEBUG, "         Pbx_DTMF_Mode: %d", getPbxDtmfMode() );
	cpDebug( LOG_DEBUG, "         Prev_URL_Size: %d", getPrevUrlSize() );

	cpDebug( LOG_DEBUG, "         PowerOnCall: %d", getPowerOnCall() );

	cpDebug( LOG_DEBUG, "--- Dial Mode ---" );
	cpDebug( LOG_DEBUG, "         Access Code Length: %d", getAccessLength() );

	cpDebug( LOG_DEBUG, "         Extension Code Length: %d", getExtLength() );
	cpDebug( LOG_DEBUG, "         Extension Code : %s", getExtAccessCode() );
	cpLog( LOG_DEBUG, "         CAR Time : %d seconds", getCarTimes() );
	cpLog( LOG_DEBUG, "         CAR Count : %d", getCarCount() );

	cpLog( LOG_DEBUG, "         Sip_Port: %s", getLocalSipPort().c_str() );
	cpLog( LOG_DEBUG, "         Sip_Transport: %s", getSipTransport().c_str() );
	cpLog( LOG_DEBUG, "       	Register_On: \"%s\"", (getRegisterOn() ? "True" : "False" ) );
	cpLog( LOG_DEBUG, "         Register_Server: %s", getRegisterServer().c_str() );
	cpLog( LOG_DEBUG, "         Register_Expires: %d ms", getRegisterExpires());
	cpLog( LOG_DEBUG, "         Proxy_Server: %s", getProxyServer().c_str() );
	cpLog( LOG_DEBUG, "         Register_Action: %s", getRegisterAction().c_str() );
	cpLog( LOG_DEBUG, "         Min_RTP_Port: %d", getMinRTPPort());
	cpLog( LOG_DEBUG, "         Max_RTP_Port: %d", getMaxRTPPort());
	cpLog( LOG_DEBUG, "       	VadString: \"%s\"", (getVadString() ? "True" : "False" ) );
	cpLog( LOG_DEBUG, "         JitterBuffer: %d", getJitterBuffer());
	cpLog( LOG_DEBUG, "         Start_CdrServer: %d", getStartCdrServer());
	cpLog( LOG_DEBUG, "         Authentication: %s", getAuthentication()?"Local" : "Off" );
	cpLog( LOG_DEBUG, "         SubscribeOn: %s", (getSubscribeOn() ? "True" : "False" ) );
	cpLog( LOG_DEBUG, "         Subscribe_Expires: %d ms", getSubscribeExpires() );
	cpLog( LOG_DEBUG, "         Subscribe_To: %s", getSubscribeTo().c_str() );
	cpLog( LOG_DEBUG, "         Cdr_Path: %s", getCdrPath().c_str() );
	cpLog( LOG_DEBUG, "         Fifo: %s", getFifo().c_str() );
	cpLog( LOG_DEBUG, "         G711_Law: %d", getG711Law() );
	cpLog( LOG_DEBUG, "		Rsvp_On: %s", (getRsvpOn() ? "True" : "False") );
	
	displayExts( );
	
	cpDebug( LOG_DEBUG, "--- ISDN Dial ---" );
	cpLog( LOG_DEBUG, "		Local Dial \t: %s", getIsdnLocalDialCode() );
	cpLog( LOG_DEBUG, "		External Dial \t: %s", getIsdnExternalDialCode() );

	cpDebug( LOG_DEBUG, "--- PSTN ---" );
	cpDebug( LOG_DEBUG, "         PSTN Standard\t: %s", (getPstnStandard()==PSTN_STANDARD_JAPAN)?"Japan":"China");
	cpDebug( LOG_DEBUG, "         CAR Time\t: %d seconds", getCarTimes() );
	cpDebug( LOG_DEBUG, "         CAR Count\t: %d", getCarCount() );
	cpDebug( LOG_DEBUG, "         PSTN Line Dial: %s", (getFxoLineMode()==FXO_LINE_DIRECT)? "Direct" :"Indirect" );
	cpDebug( LOG_DEBUG, "         PSTN Dial Mode: %s", (getFxoDialMode()==0)? "PB(DTMF)" :"DP" );
	
	cpDebug( LOG_DEBUG, "         PSTN Jump Phone Number : " );
	PstnJumpCodeList jList = getPstnJumpCodeList();
	PstnJumpCodeList::iterator jIter = jList.begin();
	while ( jIter != jList.end() )
	{
		string s = *jIter;
		cpLog( LOG_DEBUG, "\t\t : \t%s", s.c_str() );
		jIter++;
	}

	cpDebug( LOG_DEBUG, "--- DSP ---" );
	cpDebug( LOG_DEBUG, "         DTMF Duration : %d ms", getDspConfig()->dtmfDuration);
	cpDebug( LOG_DEBUG, "         DTMF Mark\t: %d ms", getDspConfig()->dtmfMarkTime);
	cpDebug( LOG_DEBUG, "         G711 \t: %s Law", (getDspConfig()->law== U_LAW_CODE)?"Mu":"A");
	cpDebug( LOG_DEBUG, "         Tone data\t: %d ms", getDspConfig()->toneLength );
	cpDebug( LOG_DEBUG, "--- ADD ---" );
	cpDebug( LOG_DEBUG, "         Provide Ringback\t: %s\t", (getProvideRingback()? "True" : "False"));
	cpDebug( LOG_DEBUG, "         Isdn Timeout\t: %d\t",getIsdnTimeout());
	cpDebug( LOG_DEBUG, "         Local Exchange\t: %s\t",getLocalExchange().c_str());
	cpDebug( LOG_DEBUG, "         AnswerRestartTimer\t: %d\t",getAnswerRestartTimer());
	cpDebug( LOG_DEBUG, "         Console Calling Timeout\t: %d\t",getConsoleCallingTimeout());
	cpDebug( LOG_DEBUG, "         Console Alert Timeout\t: %d\t",getConsoleAlertingTimeout());
	cpDebug( LOG_DEBUG, "         Conference Server\t: %s\t",getConfServer().c_str());
	cpDebug( LOG_DEBUG, "         Network RTP Rate\t: %d\t",getNetworkRtpRate());
	cpDebug( LOG_DEBUG, "         Ua Timeout\t: %d\t",getUaTimeout());
	cpDebug( LOG_DEBUG, "         Use Policy Server\t: %s\t", (getUsePolicyServer()? "True" : "False"));
	cpDebug( LOG_DEBUG, "         Provisioning Host\t: %s\t",getProvisionHost().c_str());
	cpDebug( LOG_DEBUG, "         Provisioning Port\t: %d\t",getProvisionPort());
	cpDebug( LOG_DEBUG, "         NATAddress\t: %s\t",getNATAddress().c_str());

	cpLog( LOG_DEBUG, "--- User Info ---" );
	UserInfoMap usersd = getUserInfo();
	UserInfoMap::iterator userIter = usersd.begin();
	while ( userIter != usersd.end() )
	{
		cpLog( LOG_DEBUG, "%18s : %s", userIter->first.c_str(), userIter->second.c_str() );
		userIter++;
	}
	cpLog( LOG_DEBUG, "--- Local Dial ---" );
	cpDebug( LOG_DEBUG, "         Local Device\t: %s\t",getLocalDevice().c_str());
	cpDebug( LOG_DEBUG, "         Local District\t: %s\t",getLocalDistrict().c_str());
	cpDebug( LOG_DEBUG, "         Local Country\t: %s\t",getLocalCountry().c_str());
	cpDebug( LOG_DEBUG, "         Local Branch\t: %s\t",getLocalBranch().c_str());
	
	cpLog( LOG_DEBUG, "\n\n");
}

string PbxConfiguration::getUserName()
{
	return userName;
}

string PbxConfiguration::getDisplayName()
{
	return displayName;
}

string PbxConfiguration::getPassword()
{
	return password;
}

int PbxConfiguration::getInitialDigitTimeout()
{
	return initialDigitTimeout;
}

int PbxConfiguration::getInterDigitTimeout()
{
	return interDigitTimeout;
}

DialingPlanListType& PbxConfiguration::getDialPlans()
{
	return dialPlans;    
}

SpeedDialMapType& PbxConfiguration::getSpeedDials()
{
	return speedDials;
}
UserInfoMap& PbxConfiguration::getUserInfo()
{
	return users;
}
LocalDialMapType& PbxConfiguration::getLocalDials()
{
      	return localDials;
}    

void PbxConfiguration::setUserName( string name )
{
	userName = name;
}

void PbxConfiguration::setDisplayName( string name )
{
	displayName = name;
}

void  PbxConfiguration::setPassword( string pwd )
{
	password = pwd;
}

#if 0
void PbxConfiguration::setProvideRingback( bool ringback )
{
	provideRingback = ringback;
}
#endif


void PbxConfiguration::setInitialDigitTimeout( int timeout )
{
	initialDigitTimeout = timeout;
}

void PbxConfiguration::setInterDigitTimeout( int timeout )
{
	interDigitTimeout = timeout;
}

void PbxConfiguration::addDialPattern( DialMethodsType type, string pattern )
{
	DialingPlan* plan = new DialingPlan( pattern, type );
	dialPlans.push_back( plan );
}

void PbxConfiguration::addSpeedDial( string key, string dest )
{
	speedDials[ key ] = dest;
}

void PbxConfiguration::addLocalDial( string key, string dest )
{
	localDials[ key ] = dest;
} 

void PbxConfiguration::addUserInfo(string key ,string dest)
{
	users[ key ] = dest;
}

const char* PbxConfiguration::getPhoneNumberByDeviceName(char *devicename)
{
	//PbxConfiguration::instance()->config_Mutex.lock();
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
	}
	//PbxConfiguration::instance()->config_Mutex.unlock();
	return 0;
}

int PbxConfiguration::getAnswerRestartTimer()
{
	return answerRestartTimer;
}

void PbxConfiguration::setAnswerRestartTimer(int delay)
{
	answerRestartTimer = delay;
}

string PbxConfiguration::getDeviceName() const
{
	return deviceName;
}

bool PbxConfiguration::isCallIdOn()
{
	return myCallIdOn;
}

string PbxConfiguration::getCallId()
{
	return myCallId;
}

pstn_standard_t PbxConfiguration::getPstnStandard()
{
	return pstnStandard;
}

void PbxConfiguration::setPstnStandard(pstn_standard_t _s)
{
	pstnStandard = _s;
}


int PbxConfiguration::getIsdnTimeout() const
{
	return isdnTimeout;
}

void PbxConfiguration::setCallId( const string callid )
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

void PbxConfiguration::setCallIdOn( const bool callIdOn )
{
	myCallIdOn = callIdOn;
}

void PbxConfiguration::setCarTimes(int car)
{
	carTimes = car;
}

int PbxConfiguration::getCarTimes(void)
{
	return carTimes;
}

void PbxConfiguration::setCarCount(int car)
{
	carCount = car;
}

int PbxConfiguration::getCarCount(void)
{
	return carCount;
}

void PbxConfiguration::setDeviceName( const string devicename )
{
	deviceName = devicename;
}

void PbxConfiguration::setIsdnTimeout(const int timeout)
{
	isdnTimeout = timeout;
}

void PbxConfiguration::setLogFilename(const string& filename)
{
	myLogFilename = filename;
}

string PbxConfiguration::getLogFilename() const
{
	return myLogFilename;
}

void PbxConfiguration::setGreetingFileName(const string& filename)
{
	myGreetingFileName = filename;
}

string PbxConfiguration::getGreetingFileName()const
{
	return myGreetingFileName;
}

PbxConfiguration::PbxConfiguration()
	: password("\0"),
	provideRingback(false),
	initialDigitTimeout( 16000 ),  // 16 seconds
	interDigitTimeout( 8000 ),     // 8 seconds
	answerRestartTimer( 12000 ),
	deviceName("/dev/astel"),
	myCallIdOn( false ),
	isdnTimeout( 5 ),
	accessLength(1)
{
	consoleAlertingTimeout = 30000;		/*30 seconds, 0.5 minute */
	consoleCallingTimeout = 20000;		/* 20 seconds */
}

PbxConfiguration::~PbxConfiguration()
{
}

void  PbxConfiguration::write_pid_tofile()
{
	FILE *p;
	char pid_string[1024];
	char name_string[1024];
	memset(pid_string,0,1024);
	memset(name_string,0,1024);
	p=fopen("/etc/sip/pbx.pid","w");
	if(!p)
	{
		cout<<"don't open /etc/sip/pbx.pid,exit"<<endl;
		exit(0); 
	}
	//sprintf(pid_string,"565");
	sprintf(pid_string,"%d\n",getpid());
	strcpy(name_string,"pbx\n");  
	fputs(pid_string,p);
	fputs(name_string,p);
	fclose(p);
}

void PbxConfiguration::setAccessLength( int _accessLength )
{
	accessLength = (_accessLength<MAX_ACCESS_CODE_LENGTH)?_accessLength:MAX_ACCESS_CODE_LENGTH;
}

int PbxConfiguration::getAccessLength( )
{
	return accessLength;
}

const AccessCodeMap &PbxConfiguration::getAccessCodeMap()
{
	return accessCodeMap;
}

const char *PbxConfiguration::getAccessCode(const char * plugIn)
{
	AccessCodeMap::iterator i = accessCodeMap.find( plugIn);
	if ( i != accessCodeMap.end() )
	{
		return i->second.c_str();
	}
	
	return NULL;
}

/* todo : add duplex access code check */
void PbxConfiguration::setAccessCode(string plugin, string code)
{
	PlugInsMap::iterator i = plugIns.find(plugin);
	if( i != plugIns.end() )
		accessCodeMap[plugin] = code;
}
		
void PbxConfiguration::addPlugIn( string _name, string _libName )
{
	plugIns[ _name ] = _libName;
}

PlugInsMap& PbxConfiguration::getPlugIns()
{
      	return plugIns;
}

void PbxConfiguration::setExtLength( int _length )
{
	extLength = (_length<MAX_ACCESS_CODE_LENGTH)?_length:MAX_ACCESS_CODE_LENGTH;
}

int PbxConfiguration::getExtLength( )
{
	return extLength;
}

const char *PbxConfiguration::getExtAccessCode()
{
	return extAccessCode;
}

/* todo : add duplex access code check */
void PbxConfiguration::setExtAccessCode(char *code)
{
	sprintf((char *)extAccessCode, "%s", code);
}

const char *PbxConfiguration::getIsdnLocalDialCode()
{
	return myIsdnLocalDialCode.c_str();
}
void PbxConfiguration::setIsdnLocalDialCode(const char *code)
{
	myIsdnLocalDialCode = code;
}
const char *PbxConfiguration::getIsdnExternalDialCode()
{
	return myIsdnExternalDialCode.c_str();
}
void PbxConfiguration::setIsdnExternalDialCode(const char *code)
{
	myIsdnExternalDialCode = code;
}


/* todo : add duplex EXT phone number check */
void PbxConfiguration::addExtPhoneNumber(string access_code, string phone_number)
{

	ExtMap::iterator i = extMaps.find( access_code);
	if ( i != extMaps.end() )
	{	
		i->second.push_back( phone_number);
	}
	else
	{
		list <string> exts;
		exts.push_back(phone_number);
		extMaps[access_code] = exts;
	}
}

const ExtMap& PbxConfiguration::getExtMap()
{
	return extMaps;
#if 0
	ExtMap::iterator i = extMaps.find( access_code);
	if ( i != extMaps.end() )
	{	
		return i->second;
	}
	return NULL;
#endif	
}

void PbxConfiguration::displayExts( )
{
	cpDebug( LOG_DEBUG, "--- PLUGINS ---" );
	AccessCodeMap::iterator itr = accessCodeMap.begin();
	while( itr != accessCodeMap.end() )
	{ 
		cpLog(LOG_DEBUG, "         PlugIn : %s, \tAccess Code : %s" , itr->first.c_str(), itr->second.c_str() );
		ExtMap::iterator extIter = extMaps.find(itr->second.c_str() );
		if(extIter != extMaps.end() )
		{
#if 0		
			list <string> numbers = getExtMap( itr.second.c_str() );
			if(numbers==NULL)
				return;
#endif			
			list<string>::const_iterator i = extIter->second.begin();
#if 0
#endif		
			while( i != extIter->second.end())
			{
				cpLog(LOG_DEBUG, "                  EXT : %s", (*i).c_str() );
				i++;
			}
		}
		itr++;
	}
}

int PbxConfiguration::getConsoleCallingTimeout()
{
	return consoleCallingTimeout;
}

void PbxConfiguration::setConsoleCallingTimeout( int timeMs)
{
	consoleCallingTimeout = timeMs;
}

int PbxConfiguration::getConsoleAlertingTimeout()
{
	return consoleAlertingTimeout;
}

void PbxConfiguration::setConsoleAlertingTimeout(int timeMs)
{
	consoleAlertingTimeout = timeMs;
}

FxoLineMode PbxConfiguration::getFxoLineMode()
{
	return fxoLineMode;
}

void PbxConfiguration::setFxoLineMode( FxoLineMode mode)
{
	fxoLineMode = mode;
}

FxoDialMode PbxConfiguration::getFxoDialMode()
{
	return fxoDialMode;
}

void PbxConfiguration::setFxoDialMode( FxoDialMode mode)
{
	fxoDialMode = mode;
}
string PbxConfiguration::getLocalSipPort()
{
	return sipport;
}
void PbxConfiguration::setLocalSipPort( string name )
{
	sipport = name;
}
void PbxConfiguration::setSipTransport( string name)
{
	siptransport= name;
}
string PbxConfiguration::getSipTransport()
{
	return siptransport;
}
void PbxConfiguration::setRegisterOn(const bool register_on)
{
	registeron= register_on;
}
bool PbxConfiguration::getRegisterOn()
{
	return registeron;
}
void PbxConfiguration::setRegisterServer(string name)
{
	registerserver = name;
}
string PbxConfiguration::getRegisterServer()
{
	return registerserver;
}
void PbxConfiguration::setProxyServer(string name)
{
	proxyserver = name;
}
string PbxConfiguration::getProxyServer()
{
	return proxyserver;
}
void PbxConfiguration::setRegisterExpires(int expirestime)
{
	registerexpires = expirestime;
}
int PbxConfiguration::getRegisterExpires()
{
	return registerexpires;
}
void PbxConfiguration::setRegisterAction( string name )
{
	registeraction = name;
}
string PbxConfiguration::getRegisterAction()
{
	return registeraction;
}
void PbxConfiguration::setMinRTPPort( int minport )
{
	minrtpport = minport;
}
int PbxConfiguration::getMinRTPPort()
{
	return minrtpport;
}
void PbxConfiguration::setMaxRTPPort( int maxport )
{
	maxrtpport = maxport;
}
int PbxConfiguration::getMaxRTPPort()
{
	return maxrtpport;
}
void PbxConfiguration::setVadString(const bool vadstring)
{
	vadstringpbx = vadstring;
}
bool PbxConfiguration::getVadString()
{
	return vadstringpbx;
}
void PbxConfiguration::setJitterBuffer(int jitterbuffer)
{
	jitterbufferpbx = jitterbuffer;
}
int PbxConfiguration::getJitterBuffer()
{
	return jitterbufferpbx;
}
void PbxConfiguration::setStartCdrServer(int cdrserver)
{
	startcdrserver = cdrserver;
}
int PbxConfiguration::getStartCdrServer()
{
	return startcdrserver;
}

void PbxConfiguration::setAuthentication( bool isCheck)
{
	isAuthentication = isCheck;
}
bool PbxConfiguration::getAuthentication()
{
	return isAuthentication;
}

void PbxConfiguration::setSubscribeOn(const bool subon)
{
	subscribeon = subon;
}
bool PbxConfiguration::getSubscribeOn()
{
	return subscribeon;
}
void PbxConfiguration::setSubscribeExpires(int subscribe)
{
	subscribeexpires = subscribe;  
}
int PbxConfiguration::getSubscribeExpires()
{
	return subscribeexpires;
}
void PbxConfiguration::setSubscribeTo(string name)
{
	subscribeto =  name;
}
string PbxConfiguration::getSubscribeTo()
{
	return subscribeto;
}
void PbxConfiguration::setCdrPath(string name)
{
	cdrpath = name;
}
string PbxConfiguration::getCdrPath()
{
	return cdrpath;
}
void PbxConfiguration::setFifo(string name)
{
	fifo = name;
}
string PbxConfiguration::getFifo()
{
	return fifo;
}
void PbxConfiguration::setG711Law(int glaw)
{
	g711law = glaw;
}
int PbxConfiguration::getG711Law()
{
	return g711law;
}
void PbxConfiguration::setRsvpOn(const bool rsvp)
{
	rsvpon = rsvp;
}
bool PbxConfiguration::getRsvpOn()
{
	return rsvpon;
}
void PbxConfiguration::setRegisterFrom( string from )
{
    registerFrom = from;
}  
void PbxConfiguration::setRegisterTo( string to )
{
    registerTo = to;
}  
string PbxConfiguration::getRegisterFrom()
{
    return registerFrom;
}    
string PbxConfiguration::getRegisterTo()
{
    return registerTo;
}  
void PbxConfiguration::setNATAddress(const string& address)
{
    myNATAddress = address;
}
string PbxConfiguration::getNATAddress() const
{
    return myNATAddress;
}
void PbxConfiguration::setNetworkRtpRate( int rate )
{
    networkRtpRate = rate;
}
int PbxConfiguration::getNetworkRtpRate()
{
   return networkRtpRate;
}
void PbxConfiguration::setXferMode( UaXferMode Xfermode )
{
    xferMode = Xfermode;
}
UaXferMode PbxConfiguration::getXferMode() const
{
    return xferMode;
}
void PbxConfiguration::setConfServer(const string confser)
{
	confserver = confser;
}
string PbxConfiguration::getConfServer() const
{
	return confserver;
}
void PbxConfiguration::setProvideRingback( bool ringback )
{
    provideRingback = ringback;
}
bool PbxConfiguration::getProvideRingback( )
{
	return provideRingback;
}
void PbxConfiguration::setUaTimeout(int timeout)
{
    uaTimeout = timeout;
}

int PbxConfiguration::getUaTimeout()
{
	return uaTimeout;
}

void PbxConfiguration::addVMServer(const string server)
{
    myVmServers.push_back(server);
}
vector<string> &
PbxConfiguration::getVMServers()
{
    return myVmServers;
}
void PbxConfiguration::setUsePolicyServer( const bool use )
{
    usePolicyServer = use;
}
bool PbxConfiguration::getUsePolicyServer() const
{
    return usePolicyServer;
}
void PbxConfiguration::setLocalExchange(string _localExchange)
{
    localExchange = _localExchange;
}
string PbxConfiguration::getLocalExchange() const
{
    return localExchange;
}
void PbxConfiguration::setLocalDistrict(string _localDistrict)
{
	localDistrict = _localDistrict;
}
string PbxConfiguration::getLocalDistrict() const
{	
	return localDistrict;
}
void PbxConfiguration::setLocalBranch(string _localBranch)
{
	localBranch = _localBranch;
}
string PbxConfiguration::getLocalBranch() const
{
	return localBranch;
}
void PbxConfiguration::setLocalDevice(string _localDevice)
{
	localDevice = _localDevice;
}
string PbxConfiguration::getLocalDevice() const
{	
	return localDevice;
}
void PbxConfiguration::setLocalCountry(string _localCountry)
{
	localCountry = _localCountry;
}
string PbxConfiguration::getLocalCountry() const
{	
	return localCountry;
}
void PbxConfiguration::addOthsDial(int type, const string & key, const string & dest)
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
OthsDialMapType& PbxConfiguration::getOthsDials(int type)
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


PstnJumpCodeList &PbxConfiguration::getPstnJumpCodeList()
{
	return jumpList;
}

void PbxConfiguration::addPstnJumpNumber(string jumpNumber)
{
	jumpList.push_back( jumpNumber );
}

int PbxConfiguration::getDtmfMarkTime()
{
	return dtmfMarkTime;
}

void PbxConfiguration::setDtmfMarkTime(int _time)
{
	dtmfMarkTime = _time;
}

int PbxConfiguration::getDtmfDurationTime()
{
	return dtmfDurationTime;
}

void PbxConfiguration::setDtmfDurationTime(int _time)
{
	dtmfDurationTime = _time;
}

as_dsp_config_t *PbxConfiguration::getDspConfig()
{
	return &dspConfig;
}
void PbxConfiguration::setProvisionHost(string prohost)
{
	ProHost = prohost;
}
string PbxConfiguration::getProvisionHost() const
{
	return ProHost;
}
void PbxConfiguration::setProvisionPort(int proport)
{
	ProPort = proport;
}
int PbxConfiguration::getProvisionPort() const
{
	return ProPort;
}
