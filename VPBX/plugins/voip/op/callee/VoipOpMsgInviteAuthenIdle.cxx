/*
* $Id: VoipOpMsgInviteAuthenIdle.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "VoipOpBuilder.hxx"
#include "SipAuthorization.hxx"
#include "PbxConfiguration.hxx"

using namespace Assist;

void makeAuthenString(char* md5String, const SipCommand& source, const Data& stype)
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

bool localVerifyUser(const SipAuthorization& author)
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
	string passwd = PbxConfiguration::instance()->getUserPwd(username.logData());
	if (passwd == "")
	{
		cpDebug(LOG_DEBUG, "No this User.");
		return false; // no this user
	}
	if (passwd == "-")passwd = "";
	
	realm = author.getTokenValue("realm");
	if (realm.logData()!= PbxConfiguration::instance()->getRegisterTo())return false;
	
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

void remoteVerifyUser(const SipAuthorization& author)
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


typedef map<int, FifoEventId> TimerEventMap;

//TimerEventMap OpAuthentication::tvMap;

static TimerEventMap tvMap;

/* authenticate external user when in IDLE state as callee */
const Sptr <PbxState> VoipOpMsgInviteAuthenIdle::process( const Sptr <PbxEvent> event )
{
	Sptr <SipMsg> sipMsg = isSipMsg(event);
    	if( sipMsg == 0 )
		return PBX_STATE_CONTINUE;

    	Sptr <InviteMsg> msg;
    	msg.dynamicCast( sipMsg );
   	if ( msg == 0 )
    	{
        	cpDebug(LOG_DEBUG, "Not an INVITE" );
        	return PBX_STATE_CONTINUE;
    	}
	Sptr <VoipEndPoint> vep = getVoipEndPoint(event);

	Data rhost=msg->getCallId().getHost();
	//if(rhost == "192.168.3.172");
	if(rhost == theSystem.gethostAddress(&rhost))
		return 0;

	vep->setSipContext( new SipContext);
	SipCallId sipCallId = sipMsg->getCallId();
	vep->setSipCallId( sipCallId );
			
	Sptr <SipContext> context = vep->getSipContext();
    	assert( context != 0 );
	
	context->setAuthorization(false);

	SipAuthorization sipAuthor = msg->getAuthorization();
	Data sAuthor = sipAuthor.getAuthScheme();
	if (sAuthor == "")
	{
		cpDebug(LOG_DEBUG, "****** At here, send a 401 to the source dest.");
		
		char nonce[HASHHEXLEN+3] = {0};
		makeAuthenString(nonce,*msg, "nonce");

		char opaque[HASHHEXLEN+3] = {0};
		makeAuthenString(opaque,*msg, "opaque");
		
		Data wwwAuthenString = "Digest ";
		wwwAuthenString += "realm=\"";
		wwwAuthenString += PbxConfiguration::instance()->getRegisterTo();
		wwwAuthenString += "\", ";
		wwwAuthenString += "nonce=";
		wwwAuthenString += nonce;
		wwwAuthenString += " ,opaque=";
		wwwAuthenString += opaque;

		
		//Data wwwAuthString = "Digest realm=\"wellx.org\", nonce=\"77a52d88b21f170e52379181274bf712\", opaque=\"442b0794278298749b89b65159b7dec8\"";
		StatusMsg status401(*msg, SIP_STATUS_CODE_CERROR_UNAUTHORIZED);
		status401.setWwwAuthenticate(wwwAuthenString);
		vep->getSipStack()->sendReply(status401);

		Sptr <PbxTimerEvent> AuthenTimerEvt = new PbxTimerEvent(0);
		SipCallId tempCallId = msg->getCallId();

		/* set a timer with 60s, when a UnAuthentrization INVITE reached, the was timer lanuched*/
		AuthenTimerEvt->startTimer(AuthenTimerEvt, 1000*60);//AuthenTimerEvt,

//		int deviceId = event->getDeviceId();
//		tvMap[deviceId] = AuthenTimerEvt->getTimerId();
		
		
	}
	else if(1)
	{
		cpDebug(LOG_DEBUG, "****** At here, Local Authentication needed.");

//		int deviceId = event->getDeviceId();
//		FifoEventId tempId = tvMap[deviceId];

		cpDebug(LOG_DEBUG, "Canceled the TimeEvent");
	
//		event->getFifo()->cancel(tempId);
		
		if (localVerifyUser(sipAuthor))
		{
			context->setAuthorization(true);
		}
		else
		{
			//call->setAuthorization(false);
			StatusMsg status403(*msg, 403);
			vep->getSipStack()->sendReply(status403);

			vep->reset( );
		}

	}
	else
	{
		remoteVerifyUser(sipAuthor);
	}
	
	return PBX_STATE_CONTINUE;
}

