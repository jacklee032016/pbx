/*
 * $Id: OpAuthentication.cxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "OpAuthentication.hxx"
#include "global.h"
#include "SipEvent.hxx"
#include "UaCallInfo.hxx"
#include "UaDevice.hxx"
#include "UaStateMachine.hxx"
#include "UaConfiguration.hxx"
#include "StatusMsg.hxx"
#include "Md5.hxx"
#include "digest.hxx"
#include "UaConfiguration.hxx"
#include "SipDigest.hxx"
#include "UaCallContainer.hxx"
#include "UaDevice.hxx"
#include "UaTimerEvent.hxx"
#include "HeartLessProxy.hxx"
#include "SipCallId.hxx"
#include <sys/time.h>
#include <string.h>
#include "SystemInfo.hxx"

using namespace Vocal;
using Vocal::Crypto::Md5;

TimerEventMap OpAuthentication::tvMap;


OpAuthentication::OpAuthentication()
{
}


OpAuthentication::~OpAuthentication()
{
}


const char* const
OpAuthentication::name() const
{
    return "OpAuthentication";
}


const Sptr < State >
OpAuthentication::process( const Sptr < SipProxyEvent > event )
{
	cpDebug(LOG_DEBUG, "OpAuthentication::process");
	
	Sptr < SipEvent > sipEvent;
    	sipEvent.dynamicCast( event );
    	if ( sipEvent == 0 )
    	{
    	    return 0;
  	}
   	Sptr < SipMsg > sipMsg = sipEvent->getSipMsg();
    	assert( sipMsg != 0 );

    	Sptr < InviteMsg > msg;
    	msg.dynamicCast( sipMsg );
   	if ( msg == 0 )
    	{
        	cpDebug(LOG_DEBUG, "Not an INVITE" );
        	return 0;
    	}

	Data rhost=msg->getCallId().getHost();
	if(rhost == theSystem.gethostAddress(&rhost))
		return 0;

	Sptr < UaCallInfo > call;
   	call.dynamicCast( event->getCallInfo() );
    	assert( call != 0 );

	
	call->setAuthorization(false);

	SipAuthorization sipAuthor = msg->getAuthorization();
	Data sAuthor = sipAuthor.getAuthScheme();
	if (sAuthor == "")
	{
		cpDebug(LOG_DEBUG, "****** At here, send a 401 to the source dest.");
		
		char nonce[HASHHEXLEN+3] = {0};
		MakeAuthenString(nonce,*msg, "nonce");

		char opaque[HASHHEXLEN+3] = {0};
		MakeAuthenString(opaque,*msg, "opaque");
		
		Data wwwAuthenString = "Digest ";
		wwwAuthenString += "realm=\"";
		wwwAuthenString += UaConfiguration::instance()->getRegisterTo();
		wwwAuthenString += "\", ";
		wwwAuthenString += "nonce=";
		wwwAuthenString += nonce;
		wwwAuthenString += " ,opaque=";
		wwwAuthenString += opaque;

		
		//Data wwwAuthString = "Digest realm=\"wellx.org\", nonce=\"77a52d88b21f170e52379181274bf712\", opaque=\"442b0794278298749b89b65159b7dec8\"";
		StatusMsg status401(*msg, 401);
		status401.setWwwAuthenticate(wwwAuthenString);
		event->getSipStack()->sendReply(status401);

		Sptr <UaTimerEvent> AuthenTimerEvt = new UaTimerEvent(event->getFifo());
		SipCallId tempCallId = msg->getCallId();

		Sptr <SipCallId> thisCallId = new SipCallId(tempCallId);
		AuthenTimerEvt->setCallId(thisCallId);
		
		int thisDeviceId = event->getDeviceId();
		assert(thisDeviceId >= 0);
		AuthenTimerEvt->setDeviceId(thisDeviceId);
		AuthenTimerEvt->setCallInfo(call, event->getCallContainer());

		/* set a timer with 60s, when a UnAuthentrization INVITE reached, the was timer lanuched*/
		AuthenTimerEvt->startTimer(AuthenTimerEvt, 1000*60);//AuthenTimerEvt,

		int deviceId = event->getDeviceId();
		tvMap[deviceId] = AuthenTimerEvt->getTimerId();
		
		
	}
	else if(1)
	{
		cpDebug(LOG_DEBUG, "****** At here, Local Authentication needed.");

		int deviceId = event->getDeviceId();
		FifoEventId tempId = tvMap[deviceId];

		cpDebug(LOG_DEBUG, "Canceled the TimeEvent");
		event->getFifo()->cancel(tempId);
		
		if (LocalVerifyUser(sipAuthor))
		{
			call->setAuthorization(true);
		}
		else
		{
			//call->setAuthorization(false);
			StatusMsg status403(*msg, 403);
			event->getSipStack()->sendReply(status403);
			
			Sptr < UaCallContainer > calls;
    			calls.dynamicCast( event->getCallContainer() );
   			assert( calls != 0 );
    			calls->deleteCall( *(UaDevice::instance()->getCallId( event->getDeviceId() )) ,event->getDeviceId());
   			UaDevice::instance()->setCallId( 0 ,event->getDeviceId() );
		}

	}
	else
	{
		RemoteVeriyUser(sipAuthor);
	}
	

	return 0;
    
}

void OpAuthentication::MakeAuthenString(char* md5String, const SipCommand& source, const Data& stype)
{
	struct MD5Context smd5;                 
	HASH hTmp;
	HASHHEX hTmpHex;
		
	int now = time(NULL);
	char time_stamp[30] = {0};
	sprintf(time_stamp, "%i", now);

	MD5Update(&smd5, (unsigned char*)(time_stamp), strlen(time_stamp));
	MD5Update(&smd5, (unsigned char*)(":"), 1);
	
	if (stype != "opaque")
	{
		char* smag = "magicstring";
		MD5Update(&smd5, (unsigned char*)(smag), strlen(smag));
		MD5Update(&smd5, (unsigned char*)(":"), 1);

		Data callid = source.getCallId().encode();
		MD5Update(&smd5, (unsigned char*)(callid.logData()), callid.length());
			
	}
	
	MD5Final((unsigned char*)(hTmp), &smd5);
	CvtHex(hTmp, hTmpHex);

	sprintf(md5String, "\"%s\"", hTmpHex);
	
}

bool OpAuthentication::LocalVerifyUser(const SipAuthorization& author)
{
	Data username;
	Data nonce;
    	Data realm;
   	Data alg;
    	Data opaque;
    	Data uri;
	Data response;
	

	nonce = author.getTokenValue("nonce");
	opaque = author.getTokenValue("opaque");
	
	
	if (nonce == "" || opaque =="")return false;

	username = author.getTokenValue("username");
	cpDebug(LOG_DEBUG, "UserName = %s", username.logData());
	string passwd = UaConfiguration::instance()->getUserPwd(username.logData());
	if (passwd == "")
	{
		cpDebug(LOG_DEBUG, "No this User.");
		return false; // no this user
	}
	if (passwd == "-")passwd = "";
	
	realm = author.getTokenValue("realm");
	if (realm.logData()!= UaConfiguration::instance()->getRegisterTo())return false;
	
	alg = author.getTokenValue("algorithm");
	if (alg == "")alg = "MD5"; //defaule 

	uri = author.getTokenValue("uri");
	response = author.getTokenValue("response");

	const char* method = "INVITE";
	//const char* passwd = ""; // implemention this for a configfile
	const char* qop = "";
	const char* cnonce = "";
	const char* noncecount = "";

	cpDebug(LOG_DEBUG, "ddddddddddddddddddd");

	SipDigest sipDigest;
	Data myResponse = 
		sipDigest.form_SIPdigest(nonce, 
							  username,
							  passwd,
							  method,
                                               uri,
                                               realm,
                                               qop, 
                                               cnonce, 
                                               alg,
                                               noncecount);
	
	
	cpDebug(LOG_DEBUG, "%s : %d", myResponse.logData(), myResponse.length());
	cpDebug(LOG_DEBUG, "%s : %d", response.logData(), response.length());
	
	if (myResponse == response)
	{
		cpDebug(LOG_DEBUG, "myResponse == pszResponse");
		return true;
	}
	else
	{
		cpDebug(LOG_DEBUG, "myResponse != pszResponse");
		return false;
	}
	
}

void OpAuthentication::RemoteVeriyUser(const SipAuthorization& author)
{
}

/*char* OpAuthentication::DiscardQuote(const char *ch)
{
	char *copy = new char[strlen (ch) + 1];
	if ((*ch == '\"'))
	{
	     strncpy (copy, ch + 1, strlen (ch + 1));
	     strncpy (copy + strlen (copy) - 1, "\0", 1);
	 }
	 else
	    strncpy (copy, ch, strlen (ch));
	 
	 return copy;
}
*/
