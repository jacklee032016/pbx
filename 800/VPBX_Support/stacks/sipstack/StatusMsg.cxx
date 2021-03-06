/*
* $Log: StatusMsg.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:10  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: StatusMsg.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include "support.hxx"
#include "InviteMsg.hxx"
#include "SipAllow.hxx"
#include "SipContact.hxx"
#include "SipOsp.hxx"
#include "SipRequire.hxx"
#include "SipRetryAfter.hxx"
#include "SipRoute.hxx"
#include "SipSubsNotifyEvent.hxx"
#include "SipUnsupported.hxx"
#include "SipUserAgent.hxx"
#include "SipVia.hxx"
#include "StatusMsg.hxx"
#include "RandomHex.hxx"

using namespace Assist;


#define NUM_TAG_RANDOMNESS 4 //32 bits of randomness.


StatusMsg::StatusMsg()
        :
        SipMsg(),
        statusLine()
{
   
}


StatusMsg::StatusMsg(const StatusMsg& src)
        :
        SipMsg(src),
        statusLine(src.statusLine)
{
}


StatusMsg::~StatusMsg()
{
}


StatusMsg::StatusMsg(const Data & data) throw (SipParserException&)
        :
        SipMsg(),
        statusLine()
{
    try
    {
        decode(data);
    }
    catch (SipParserException& e)
    {
        cpLog(LOG_ERR, "Failed to parse the SIP msg, since: %s", 
              e.getDescription().c_str());
        throw SipParserException(e.getDescription(),
                                 __FILE__,
                                 __LINE__);
    }
}


Method StatusMsg::getType() const
{
    return SIP_STATUS;
}


const StatusMsg& StatusMsg::operator =(const StatusMsg& src)
{
    if ( &src != this)
    {
        *(static_cast < SipMsg* > (this)) = src;
	statusLine = src.statusLine;
    }
    return (*this);
}


bool StatusMsg::operator ==(const StatusMsg& src)
{
    return SipMsg::operator==(src);
}


StatusMsg::StatusMsg(SipCommand& command, int statusCode)
        :
        SipMsg(),
        statusLine()
{
    //  set the SipStatusLine.

    statusLine.setStatusCode(statusCode);

    Method meth = command.getType();

    //responses to all messages will have the below mandatory.

    copyHeader(command, SIP_VIA_HDR);
    copyHeader(command, SIP_FROM_HDR);
    copyHeader(command, SIP_TO_HDR);
    copyHeader(command, SIP_CALLID_HDR);
    copyHeader(command, SIP_CSEQ_HDR);

    if ( statusCode > 100 && statusCode < 500 )
    {
        if (!(command.getTo().getTag().length()) )
        {
	    // add the To tag only if the command does not contain a To tag already
            Data statusToTag = RandomHex::get(NUM_TAG_RANDOMNESS);
	    cpDebug(LOG_DEBUG_STACK, "To tag generated by stack is : %s", statusToTag.logData());  
	    
	    //set this in the To field.
	    SipTo tempTo = getTo();
	    tempTo.setTag(statusToTag);
	    setTo(tempTo);
	}
    }
    if ( statusCode > 100 && statusCode <= 200)
    {
        //copy record route.
        if (command.getNumRecordRoute())
        {
            copyHeader(command, SIP_RECORD_ROUTE_HDR);
        }

        if (meth == SIP_SUBSCRIBE)
        {
            //if subscribe, copy all the event headers.
            copySubsNotifyEvent(command);
        }
    }

    if ((meth == SIP_INVITE ) && ( statusCode == 302))
    {
        copyDiversionList( command );
    }
    //set content Length as zero. Will be later reset if any mime information
    //is added to this status message.
    setContentLength(ZERO_CONTENT_LENGTH);
}

    
bool 
StatusMsg::isStatusCodes(const Data& code)
{
    if ( (code.convertInt() >= 100) && (code.convertInt() <= 700) )
    {
        return true;
    }
    else
    {
        return false;
    }
}


bool 
StatusMsg::isprotocolSIP(const Data& pheader)
{
    bool ret = false;
    Data header = pheader;
    Data protoVersion;
    int value = header.match("/", &protoVersion, true);
    if (value == FOUND)
    {
        if (protoVersion == DEFAULT_PROTOCOL)
        {
            ret = true;
        }
        else
        {
            ret = false;
        }
    }
    else
    {
        ret = false;
    }
    return ret;
}


void 
StatusMsg::parseStartLine(const Data& plin)
{ 
  Data lin = plin;
  Data line1 = plin;
  int fieldvalue1;
  Data header1;
  fieldvalue1 = lin.match(SP, &header1, false);
  if (fieldvalue1 == FOUND)
    {
      if (fieldvalue1 == FOUND)
	{
	  if (isprotocolSIP(header1))
	    {
	      //scan till the second SP, which will give the status-code.
	      Data lstatusLine = line1;
	      lstatusLine.match(SP, &header1, true);
	      lstatusLine.match(SP, &header1, true);
	      if (isStatusCodes(header1))
		{
		  setStatusLine(line1);
		}
	    }
	}
    }
  
}


void
StatusMsg::decode(const Data & sltdata)
{
    // new, revised decode

    // get the first line, and parse

    Data rawMsg = sltdata;

    bool noMatch = false;
    Data line = rawMsg.getLine(&noMatch);

    parseStartLine(line);

    try
    {
        // pass the rest up to SipMsg::parseHeaders()
        SipMsg::parse(rawMsg);
    }
    catch(VException& e)
    {
        cpLog(LOG_ERR, "Failed to parse the SIP msg, since: %s", 
              e.getDescription().c_str());
        throw SipParserException(e.getDescription(),
                                 __FILE__,
                                 __LINE__);
    }
    catch(...)
    {
        cpLog(LOG_ERR, "Failed to parse the SIP msg for unknown reason");
        throw SipParserException("unknown reason",
                                 __FILE__,
                                 __LINE__);
    }
}

/*------------------------------ SipStatusLine ------------------------------*/


const SipStatusLine&
StatusMsg::getStatusLine() const
{
    return statusLine;
}

void StatusMsg::setStatusLine( const SipStatusLine& newstatusLine)
{
    statusLine = newstatusLine;
}

    
void StatusMsg::setStatusLine( const Data& newstatusLine)
{
    statusLine.decode(newstatusLine);
}


void StatusMsg::setStatusDetails()
{
}


void StatusMsg::setReasonPhrase(const Data& reason)
{
    statusLine.setReasonPhrase(reason);
}

Data StatusMsg::getReasonPhrase() const
{
    return (statusLine.getReasonPhrase());
}


    
/*------------------------------ SipWarning ------------------------------*/


int StatusMsg::getNumWarning() const
{
    return myHeaderList.getNumHeaders(SIP_WARNING_HDR);
}


const SipWarning& StatusMsg::getWarning( int i /*Default Arguments */) const
{
    Sptr<SipWarning> x;
    myHeaderList.getParsedHeader(x, SIP_WARNING_HDR, i);
    return *x;
}

    
void StatusMsg::setWarning(const SipWarning& item, int index /*Default Arguments */)
{
    Sptr<SipHeader> x = new SipWarning(item);
    myHeaderList.appendHeader(SIP_WARNING_HDR, x, index);
}
    
    
void StatusMsg::setWarning(const Data& item, int index /*Default Arguments */)
{
    Sptr<SipHeader> x = new SipWarning(item);
    myHeaderList.appendHeader(SIP_WARNING_HDR, x, index);
}
    

void StatusMsg::setNumWarning(int index)
{
    myHeaderList.setNumHeaders(SIP_WARNING_HDR, index);
}


/*------------------------------ SipUnsupported ------------------------------*/


int StatusMsg::getNumUnsupported() const
{
    return myHeaderList.getNumHeaders(SIP_UNSUPPORTED_HDR);
}


const SipUnsupported&
StatusMsg::getUnsupported( int i /*default Arguments*/) const
{
    Sptr<SipUnsupported> x;
    myHeaderList.getParsedHeader(x, SIP_UNSUPPORTED_HDR, i);
    return *x;
}


void StatusMsg::setUnsupported(const SipUnsupported& item, int index /*Default Arguments */ )
{
    Sptr<SipHeader> x = new SipUnsupported(item);
    myHeaderList.appendHeader(SIP_UNSUPPORTED_HDR, x, index);
}
    
    
void StatusMsg::setUnsupported(const Data& item, int index /*Default Arguments */ )
{
    Sptr<SipHeader> x = new SipUnsupported(item);
    myHeaderList.appendHeader(SIP_UNSUPPORTED_HDR, x, index);
}
    
    
void StatusMsg::setNumUnsupported(int index)
{
    myHeaderList.setNumHeaders(SIP_UNSUPPORTED_HDR, index);
}


/*-------------------------- SipServer --------------------------*/


const SipServer&
StatusMsg::getServer() const
{
    Sptr<SipServer> x;
    myHeaderList.getParsedHeader(x, SIP_SERVER_HDR);
    return *x;
}
    
    
void StatusMsg::setServer( const SipServer& newserver)
{
    Sptr<SipServer> x;
    myHeaderList.getParsedHeader(x, SIP_SERVER_HDR);
    *x = newserver;
}
    

void StatusMsg::setServer( const Data& newserver)
{
    Sptr<SipServer> x;
    myHeaderList.getParsedHeader(x, SIP_SERVER_HDR);
    x->decode(newserver);
}


/*------------------------------ SipInReplyTo ------------------------------*/


int StatusMsg::getNumInReplyTo() const
{
    return myHeaderList.getNumHeaders(SIP_IN_REPLY_TO_HDR);
}

const SipInReplyTo&
StatusMsg::getInReplyTo( int i /*Default Arguments */) const
{
    Sptr<SipInReplyTo> x;
    myHeaderList.getParsedHeader(x, SIP_IN_REPLY_TO_HDR, i);
    return *x;
}

void StatusMsg::setInReplyTo(const SipInReplyTo& item, int index /*Default Arguments */)
{
    Sptr<SipHeader> x = new SipInReplyTo(item);
    myHeaderList.appendHeader(SIP_IN_REPLY_TO_HDR, x, index);
}
    
    
void StatusMsg::setInReplyTo(const Data& item, int index)
{
    Sptr<SipHeader> x = new SipInReplyTo(item);
    myHeaderList.appendHeader(SIP_IN_REPLY_TO_HDR, x, index);
}


void StatusMsg::setNumInReplyTo(int index)
{
    myHeaderList.setNumHeaders(SIP_IN_REPLY_TO_HDR, index);
}


/*-------------------------- SipProxyAuthenticate --------------------------*/


const SipProxyAuthenticate&
StatusMsg::getProxyAuthenticate() const
{
    Sptr<SipProxyAuthenticate> x;
    myHeaderList.getParsedHeader(x, SIP_PROXY_AUTHENTICATE_HDR);
    return *x;
}

void StatusMsg::setProxyAuthenticate( const SipProxyAuthenticate& newproxyAuth)
{
    Sptr<SipProxyAuthenticate> x;
    myHeaderList.getParsedHeader(x, SIP_PROXY_AUTHENTICATE_HDR);
    *x = newproxyAuth;
}
    
    
void StatusMsg::setProxyAuthenticate( const Data& newproxyAuth)
{
    Sptr<SipProxyAuthenticate> x;
    myHeaderList.getParsedHeader(x, SIP_PROXY_AUTHENTICATE_HDR);
    x->decode(newproxyAuth);
}    


/*--------------------------- SipWwwAuthenticate ---------------------------*/


const SipWwwAuthenticate&
StatusMsg::getWwwAuthenticate() const
{
    Sptr<SipWwwAuthenticate> x;
    myHeaderList.getParsedHeader(x, SIP_WWW_AUTHENTICATE_HDR);
    return *x;
}
    
    
void StatusMsg::setWwwAuthenticate( const SipWwwAuthenticate& newAuthenticate)
{
    Sptr<SipWwwAuthenticate> x;
    myHeaderList.getParsedHeader(x, SIP_WWW_AUTHENTICATE_HDR);
    *x = newAuthenticate;
}
    
void StatusMsg::setWwwAuthenticate( const Data& newAuthenticate)
{
    Sptr<SipWwwAuthenticate> x;
    myHeaderList.getParsedHeader(x, SIP_WWW_AUTHENTICATE_HDR);
    x->decode(newAuthenticate);
}


/*------------------------------  SipSession ------------------------------*/


const SipSession&
StatusMsg::getSession() const
{
    Sptr<SipSession> x;
    myHeaderList.getParsedHeader(x, SIP_SESSION_HDR);
    return *x;
}


void StatusMsg::setSession(const SipSession& newSession)
{
    Sptr<SipSession> x;
    myHeaderList.getParsedHeader(x, SIP_SESSION_HDR);
    *x = newSession;
}

    
void StatusMsg::setSession(const Data& newSession)
{
    Sptr<SipSession> x;
    myHeaderList.getParsedHeader(x, SIP_SESSION_HDR);
    x->decode(newSession);
}    


// -------------------- Authentication / Challenge stuff --------------------


void
StatusMsg::challengeBasicProxyAuthenticate(Data realm)
{
    SipProxyAuthenticate sipAuthHeader;

    // basic
    sipAuthHeader.setAuthScheme( AUTH_BASIC );
    sipAuthHeader.setRealmValue ( realm );

    setProxyAuthenticate( sipAuthHeader );
}


void
StatusMsg::challengeDigestProxyAuthenticate(Data realm, Data nonce)
{
    // xxx should add "sip:" to front of host as this is a URL
    SipProxyAuthenticate sipAuthHeader;

    // set digest details in ProxyAuthentication header
    sipAuthHeader.setAuthTokenDetails( REALM,
                                       realm );
    
    sipAuthHeader.setAuthTokenDetails( ALGORITHM, MD5 );
    sipAuthHeader.setAuthTokenDetails( NONCE, nonce );
    sipAuthHeader.setAuthTokenDetails( "domain", realm );
    sipAuthHeader.setAuthScheme( AUTH_DIGEST );

    setProxyAuthenticate( sipAuthHeader );
}


void
StatusMsg::challengeBasicWwwAuthenticate(Data realm)
{
    SipWwwAuthenticate sipAuthHeader;

    // basic
    sipAuthHeader.setAuthScheme( AUTH_BASIC );
    sipAuthHeader.setRealmValue ( realm );

    setWwwAuthenticate( sipAuthHeader );
}


void
StatusMsg::challengeDigestWwwAuthenticate(Data realm, Data nonce)
{
    SipWwwAuthenticate sipAuthHeader;

    // set digest details in ProxyAuthentication header
    sipAuthHeader.setAuthTokenDetails( REALM,
                                       realm );
    
    sipAuthHeader.setAuthTokenDetails( ALGORITHM, MD5 );
    sipAuthHeader.setAuthTokenDetails( NONCE, nonce );
    // xxx ? 
//    sipAuthHeader.setAuthTokenDetails( "domain", realm );
    sipAuthHeader.setAuthScheme( AUTH_DIGEST );

    setWwwAuthenticate( sipAuthHeader );
}


// ------------------ General StatusMsg stuff -----------------


Data 
StatusMsg::encode() const
{
    Data status;
    //SK explained in Invitemsg::encode--setContentLengthValue()
    status = statusLine.encode();

    encodeBody(&status);

    return status;
}


///Check to see if message is a candidate for retransmission
bool 
StatusMsg::toBeRetransmitted() const
{
    int statusCode = getStatusLine().getStatusCode();
    if ( ( getCSeq().getMethod() == INVITE_METHOD) &&
         ( statusCode >= 200)
        )
    {
        return(true);
    }
    return false;
}


const SipSessionExpires& 
StatusMsg::getSessionExpires() const
{
    Sptr<SipSessionExpires> x;
    myHeaderList.getParsedHeader(x, SIP_SESSION_EXPIRES_HDR);
    return *x;
}


const SipMinSE& 
StatusMsg::getMinSE() const 
{
    Sptr<SipMinSE> x;
    myHeaderList.getParsedHeader(x, SIP_MIN_SE_HDR);
    return *x;
}


void 
StatusMsg::setSessionExpires(const SipSessionExpires& sess)
{
    int statusCode = getStatusLine().getStatusCode();
    assert((statusCode == 200) || (statusCode == 422));
    Sptr<SipSessionExpires> x;
    myHeaderList.getParsedHeader(x, SIP_SESSION_EXPIRES_HDR);
    *x = sess;
}


void 
StatusMsg::setMinSE(const SipMinSE& minSe)
{
    int statusCode = getStatusLine().getStatusCode();
    assert((statusCode == 200) || (statusCode == 422));
    Sptr<SipMinSE> x;
    myHeaderList.getParsedHeader(x, SIP_MIN_SE_HDR);
    *x = minSe;
}
