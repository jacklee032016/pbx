/*
 * $Id: SipHeader.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */

#include "global.h"
#include "SipHeader.hxx"
#include "SipAccept.hxx"
#include "SipAcceptEncoding.hxx"
#include "SipAcceptLanguage.hxx"
#include "SipAllow.hxx"
#include "SipAlso.hxx"
#include "SipAuthorization.hxx"
#include "SipCallId.hxx"
#include "SipCallLeg.hxx"
#include "SipContact.hxx"
#include "SipContentData.hxx"
#include "SipContentDisposition.hxx"
#include "SipContentEncoding.hxx"
#include "SipContentLanguage.hxx"
#include "SipContentLength.hxx"
#include "SipContentType.hxx"
#include "SipDate.hxx"
#include "SipCSeq.hxx"
#include "SipDiversion.hxx"
#include "SipEncryption.hxx"
#include "SipExpires.hxx"
#include "SipFrom.hxx"
#include "SipHide.hxx"
#include "SipInReplyTo.hxx"
#include "SipMaxForwards.hxx"
#include "SipMimeVersion.hxx"
#include "SipOrganization.hxx"
#include "SipOspAuthorization.hxx"
#include "SipPriority.hxx"
#include "SipProxyAuthorization.hxx"
#include "SipProxyAuthenticate.hxx"
#include "SipProxyRequire.hxx"
#include "SipRecordRoute.hxx"
#include "SipReferredBy.hxx"
#include "SipReferTo.hxx"
#include "SipRequire.hxx"
#include "SipReplaces.hxx"
#include "SipResponseKey.hxx"
#include "SipRetryAfter.hxx"
#include "SipRoute.hxx"
#include "SipServer.hxx"
#include "SipSession.hxx"
#include "SipSubject.hxx"
#include "SipSubsNotifyEvent.hxx"
#include "SipSupported.hxx"
#include "SipTimestamp.hxx"
#include "SipTo.hxx"
#include "SipTransferTo.hxx"
#include "SipUnsupported.hxx"
#include "SipUserAgent.hxx"
#include "SipVia.hxx"
#include "SipWarning.hxx"
#include "SipSessionExpires.hxx"
#include "SipWwwAuthenticate.hxx"
#include "SipSessionExpires.hxx"
#include "SipMinSE.hxx"

using namespace Vocal;


// We're going to maintain two tables, one for easy maintenance
// and one for use. The first table, of SipHeaderTableEntry, is
// constructed from native types. The second table, of SipHeaderNames,
// is constructed of composite types. 
//
// The reason for the two tables is that the Windows compiler (version 6.0), 
// cannot initize an array with compositite types without choking.
//
struct SipHeaderTableEntry {
        SipHeaderType type;
        const char * lowerName;
        const char * upperName;
        const char * shortName;
};

struct SipHeaderNames {
        SipHeaderType type;
        Data lowerName;
        Data upperName;
        Data shortName;

        SipHeaderNames() {}
        SipHeaderNames(const SipHeaderTableEntry & src)
            :   type(src.type), lowerName(src.lowerName), 
                upperName(src.upperName), shortName(src.shortName)
        {}
};


// This table is used to populate the sipHeaderNames table, which is
// used to determine which type a given sipHeader is, particularly for
// the purpose of constructing a specific type of SipHeader based on
// its text.

// This table should be maintained in alphabetical order.  The entry
// you add needs to be in the exact same place as the entry in
// SipHeaderType in SipHeader.hxx.  


static const SipHeaderTableEntry sipHeaderTable[sipHeaderNameCount] =
{
    { SIP_ACCEPT_HDR,             "accept",             "Accept"          ,"" },
    { SIP_ACCEPT_ENCODING_HDR,    "accept-encoding",    "Accept-Encoding" ,"" },
    { SIP_ACCEPT_LANGUAGE_HDR,    "accept-language",    "Accept-Language" ,"" },
    { SIP_ALLOW_HDR,              "allow",              "Allow"           ,"" },
    { SIP_ALSO_HDR,               "also",               "Also"            ,"" },
    { SIP_AUTHORIZATION_HDR,      "authorization",      "Authorization"   ,"" },
    { SIP_CALLID_HDR,             "call-id",            "Call-ID"         ,"i" },
    { SIP_CALL_LEG_HDR,           "call-leg",           "Call-Leg"        ,""},
    { SIP_CONTACT_HDR,            "contact",            "Contact"         ,"m"},
    { SIP_CONTENT_DATA_HDR,       "content-data",       "Content-Data"    ,""},
    { SIP_CONTENT_DISPOSITION_HDR, "content-disposition",                 
                                                     "Content-Disposition","" },
    { SIP_CONTENT_ENCODING_HDR,   "content-encoding",   "Content-Encoding","e" },
    { SIP_CONTENT_LANGUAGE_HDR,   "content-language",   "Content-Language","" },
    { SIP_CONTENT_LENGTH_HDR,     "content-length",     "Content-Length"  ,"l" },
    { SIP_CONTENT_TYPE_HDR,       "content-type",       "Content-Type"    ,"c" },
    { SIP_CSEQ_HDR,               "cseq",               "CSeq"            ,"" },
    { SIP_DATE_HDR,               "date",               "Date"            ,"" },
    { SIP_DIVERSION_HDR,          "diversion",          "Diversion"       ,"" },
//    { SIP_DIVERSION_HDR,          "cc-diversion",       "CC-Diversion"    ,"" },
    { SIP_ENCRYPTION_HDR,         "encryption",         "Encryption"      ,"" },
    { SIP_EXPIRES_HDR,            "expires",            "Expires"         ,"" },
    { SIP_EVENT_HDR,              "event",              "Event"           ,"" },
    { SIP_FROM_HDR,               "from",               "From"            ,"f" },
    { SIP_HIDE_HDR,               "hide",               "Hide"            ,"" },
    { SIP_IN_REPLY_TO_HDR,        "in-reply-to",        "In-Reply-To"     ,"" },
    { SIP_MAX_FORWARDS_HDR,       "max-forwards",       "Max-Forwards"    ,"" },
    { SIP_MIME_VERSION_HDR,       "mime-version",       "Mime-Version"    ,"" },
    { SIP_ORGANIZATION_HDR,       "organization",       "Organization"    ,"" },
    { SIP_OSP_AUTHORIZATION_HDR,  "osp-authorization-token",             
                                                        "OSP-Authorization-Token" ,"" },
    { SIP_PRIORITY_HDR,           "priority",           "Priority"        ,"" },
    { SIP_PROXY_AUTHENTICATE_HDR, "proxy-authenticate", "Proxy-Authenticate" ,""  },
    { SIP_PROXY_AUTHORIZATION_HDR, "proxy-authorization", 
                                                        "Proxy-Authorization" ,""},
    { SIP_PROXY_REQUIRE_HDR,      "proxy-require",      "Proxy-Require"    ,""},
    { SIP_RECORD_ROUTE_HDR,       "record-route",       "Record-Route"     ,""},
    { SIP_REFERRED_BY_HDR,        "referred-by",        "Referred-By"      ,"b"},
    { SIP_REFER_TO_HDR,           "refer-to",           "Refer-To"         ,"r"},
    { SIP_REPLACES_HDR,           "replaces",           "Replaces"         ,""},
    { SIP_REQUIRE_HDR,            "require",            "Require"          ,""},
    { SIP_RESPONSE_KEY_HDR,       "response-key",       "Response-Key"     ,""},
    { SIP_RETRY_AFTER_HDR,        "retry-after",        "Retry-After"      ,""},
    { SIP_ROUTE_HDR,              "route",              "Route"            ,""},
    { SIP_SERVER_HDR,             "server",             "Server"           ,""},
    { SIP_SESSION_HDR,            "session",            "Session"          ,""},
    { SIP_SUBJECT_HDR,            "subject",            "Subject"          ,"s"},
    { SIP_SUBSCRIPTION_NOTIFY_HDR, "subscription-notify",                  
                                                        "Subscription-Notify" ,"" },
    { SIP_SUPPORTED_HDR,          "supported",          "Supported"      ,"k"  },
    { SIP_TIMESTAMP_HDR,          "timestamp",          "Timestamp"      ,"" },
    { SIP_TO_HDR,                 "to",                 "To"             ,"t" },
    { SIP_TRANSFER_TO_HDR,        "transfer-to",        "Transfer-To"    ,"" },
    { SIP_UNSUPPORTED_HDR,        "unsupported",        "Unsupported"    ,"" },
    { SIP_USER_AGENT_HDR,         "user-agent",         "User-Agent"     ,"" },
    { SIP_VIA_HDR,                "via",                "Via"            ,"v" },
    { SIP_WARNING_HDR,            "warning",            "Warning"         ,"" },
    { SIP_WWW_AUTHENTICATE_HDR,   "www-authenticate",   "WWW-Authenticate" ,""},
    { SIP_SESSION_EXPIRES_HDR,   "session-expires",   "Session-Expires" ,"x"},
    { SIP_MIN_SE_HDR,   "min-se",   "Min-SE" ,""},
    { SIP_UNKNOWN_HDR,            "",                   "" ,""}  // leave this last
};


static SipHeaderNames sipHeaderNames[sipHeaderNameCount];

// This is evil, but it guarantees initialization of the 
// sipHeaderNames after it has been allocated and initialized.
//
struct SipHeaderInit
{
    public:
        SipHeaderInit()
        {
            SipHeader::init();
        }
};

static SipHeaderInit sipHeaderInit;


void SipHeader::init()
{
    for ( int i = 0; i < sipHeaderNameCount; ++i )
    {
        assert(static_cast<int>(sipHeaderTable[i].type) == i);
        sipHeaderNames[i] = SipHeaderNames(sipHeaderTable[i]);
    }
}


    
SipHeaderType 
Vocal::headerTypeDecode(const Data& headerName)
{
    int jump = 0;
    
    while (jump < sipHeaderNameCount)
    {
        if ((headerName == sipHeaderNames[jump].lowerName) || 
            (headerName == sipHeaderNames[jump].shortName))
            return sipHeaderNames[jump].type;
        else
            ++jump;
    }
    return SIP_UNKNOWN_HDR;
}


Data 
Vocal::headerTypeEncode(const SipHeaderType type)
{   
    return sipHeaderNames[type].upperName;
}


SipHeader*
SipHeader::decodeHeader( SipHeaderType type, const Data& value )
{
    SipHeader* header = 0;

    switch(type)
    {
    case    SIP_ACCEPT_HDR:
        if(value == "")
        {
            header = new SipAccept();
        }
        else
        {
            header = new SipAccept(value);
        }
        break;
    case    SIP_ACCEPT_ENCODING_HDR:
        if(value == "")
        {
            header = new SipAcceptEncoding();
        }
        else
        {
            header = new SipAcceptEncoding(value);
        }
        break;
    case    SIP_ACCEPT_LANGUAGE_HDR:
        if(value == "")
        {
            header = new SipAcceptLanguage();
        }
        else
        {
            header = new SipAcceptLanguage(value);
        }
        break;
    case    SIP_ALLOW_HDR:
        if(value == "")
        {
            header = new SipAllow();
        }
        else
        {
            header = new SipAllow(value);
        }
        break;
    case    SIP_ALSO_HDR:
        if(value == "")
        {
            header = new SipAlso();
        }
        else
        {
            header = new SipAlso(value);
        }
        break;
    case    SIP_AUTHORIZATION_HDR:
        if(value == "")
        {
            header = new SipAuthorization();
        }
        else
        {
            header = new SipAuthorization(value);
        }
        break;
    case    SIP_CALLID_HDR:
        if(value == "")
        {
            header = new SipCallId();
        }
        else
        {
            header = new SipCallId(value);
        }
        break;
    case    SIP_CALL_LEG_HDR:
        if(value == "")
        {
            header = new SipCallLeg();
        }
        else
        {
            header = new SipCallLeg(value);
        }
        break;
    case    SIP_CONTACT_HDR:
        if(value == "")
        {
            header = new SipContact();
        }
        else
        {
            header = new SipContact(value);
        }
        break;
    case    SIP_CONTENT_DATA_HDR:
        break;
    case    SIP_CONTENT_DISPOSITION_HDR:
        if(value == "")
        {
            header = new SipContentDisposition();
        }
        else
        {
            header = new SipContentDisposition(value);
        }
        break;
    case    SIP_CONTENT_ENCODING_HDR:
        if(value == "")
        {
            header = new SipContentEncoding();
        }
        else
        {
            header = new SipContentEncoding(value);
        }
        break;
    case    SIP_CONTENT_LANGUAGE_HDR:
        if(value == "")
        {
            header = new SipContentLanguage();
        }
        else
        {
            header = new SipContentLanguage(value);
        }
        break;
    case    SIP_CONTENT_LENGTH_HDR:
        if(value == "")
        {
            header = new SipContentLength();
        }
        else
        {
            header = new SipContentLength(value);
        }
        break;
    case    SIP_CONTENT_TYPE_HDR:
        if(value == "")
        {
            header = new SipContentType();
        }
        else
        {
            header = new SipContentType(value);
        }
        break;
    case    SIP_CSEQ_HDR:
        if(value == "")
        {
            header = new SipCSeq();
        }
        else
        {
            header = new SipCSeq(value);
        }
        break;
    case    SIP_DATE_HDR:
        if(value == "")
        {
            header = new SipDate();
        }
        else
        {
            header = new SipDate(value);
        }
        break;
    case    SIP_DIVERSION_HDR:
        if(value == "")
        {
            header = new SipDiversion();
        }
        else
        {
            header = new SipDiversion(value);
        }
        break;
    case    SIP_ENCRYPTION_HDR:
        if(value == "")
        {
            header = new SipEncryption();
        }
        else
        {
            header = new SipEncryption(value);
        }
        break;
    case    SIP_EXPIRES_HDR:
        if(value == "")
        {
            header = new SipExpires();
        }
        else
        {
            header = new SipExpires(value);
        }
        break;
    case    SIP_EVENT_HDR:
        if(value == "")
        {
            header = new SipSubsNotifyEvent();
        }
        else
        {
            header = new SipSubsNotifyEvent(value);
        }
        break;
    case    SIP_FROM_HDR:
        if(value == "")
        {
            header = new SipFrom();
        }
        else
        {
            header = new SipFrom(value);
        }
        break;
    case    SIP_HIDE_HDR:
        if(value == "")
        {
            header = new SipHide();
        }
        else
        {
            header = new SipHide(value);
        }
        break;
    case    SIP_IN_REPLY_TO_HDR:
        if(value == "")
        {
            header = new SipInReplyTo();
        }
        else
        {
            header = new SipInReplyTo(value);
        }
        break;
    case    SIP_MAX_FORWARDS_HDR:
        if(value == "")
        {
            header = new SipMaxForwards();
        }
        else
        {
            header = new SipMaxForwards(value);
        }
        break;
    case    SIP_MIME_VERSION_HDR:
        if(value == "")
        {
            header = new SipMimeVersion();
        }
        else
        {
            header = new SipMimeVersion(value);
        }
        break;
    case    SIP_ORGANIZATION_HDR:
        if(value == "")
        {
            header = new SipOrganization();
        }
        else
        {
            header = new SipOrganization(value);
        }
        break;
    case    SIP_OSP_AUTHORIZATION_HDR:
        if(value == "")
        {
            header = new SipOspAuthorization();
        }
        else
        {
            header = new SipOspAuthorization(value);
        }
        break;
    case    SIP_PRIORITY_HDR:
        if(value == "")
        {
            header = new SipPriority();
        }
        else
        {
            header = new SipPriority(value);
        }
        break;
    case    SIP_PROXY_AUTHENTICATE_HDR:
        if(value == "")
        {
            header = new SipProxyAuthenticate();
        }
        else
        {
            header = new SipProxyAuthenticate(value);
        }
        break;
    case    SIP_PROXY_AUTHORIZATION_HDR:
        if(value == "")
        {
            header = new SipProxyAuthorization();
        }
        else
        {
            header = new SipProxyAuthorization(value);
        }
        break;
    case    SIP_PROXY_REQUIRE_HDR:
        if(value == "")
        {
            header = new SipProxyRequire();
        }
        else
        {
            header = new SipProxyRequire(value);
        }
        break;
    case    SIP_RECORD_ROUTE_HDR:
        if(value == "")
        {
            header = new SipRecordRoute();
        }
        else
        {
            header = new SipRecordRoute(value);
        }
        break;
    case    SIP_REFERRED_BY_HDR:
        if(value == "")
        {
            header = new SipReferredBy();
        }
        else
        {
            header = new SipReferredBy(value);
        }
        break;
    case    SIP_REFER_TO_HDR:
        if(value == "")
        {
            header = new SipReferTo();
        }
        else
        {
            header = new SipReferTo(value);
        }
        break;
    case    SIP_REPLACES_HDR:
        if(value == "")
        {
            header = new SipReplaces();
        }
        else
        {
            header = new SipReplaces(value);
        }
        break;
    case    SIP_REQUIRE_HDR:
        if(value == "")
        {
            header = new SipRequire();
        }
        else
        {
            header = new SipRequire(value);
        }
        break;
    case    SIP_RESPONSE_KEY_HDR:
        if(value == "")
        {
            header = new SipResponseKey();
        }
        else
        {
            header = new SipResponseKey(value);
        }
        break;
    case    SIP_RETRY_AFTER_HDR:
        if(value == "")
        {
            header = new SipRetryAfter();
        }
        else
        {
            header = new SipRetryAfter(value);
        }
        break;
    case    SIP_ROUTE_HDR:
        if(value == "")
        {
            header = new SipRoute();
        }
        else
        {
            header = new SipRoute(value);
        }
        break;
    case    SIP_SERVER_HDR:
        if(value == "")
        {
            header = new SipServer();
        }
        else
        {
            header = new SipServer(value);
        }
        break;
    case    SIP_SESSION_HDR:
        if(value == "")
        {
            header = new SipSession();
        }
        else
        {
            header = new SipSession(value);
        }
        break;
    case    SIP_SUBJECT_HDR:
        if(value == "")
        {
            header = new SipSubject();
        }
        else
        {
            header = new SipSubject(value);
        }
        break;
    case    SIP_SUBSCRIPTION_NOTIFY_HDR:
        if(value == "")
        {
            header = new SipSubsNotifyEvent();
        }
        else
        {
            header = new SipSubsNotifyEvent(value);
        }
        break;
    case    SIP_SUPPORTED_HDR:
        if(value == "")
        {
            header = new SipSupported();
        }
        else
        {
            header = new SipSupported(value);
        }
        break;
    case    SIP_TIMESTAMP_HDR:
        if(value == "")
        {
            header = new SipTimestamp();
        }
        else
        {
            header = new SipTimestamp(value);
        }
        break;
    case    SIP_TO_HDR:
        if(value == "")
        {
            header = new SipTo();
        }
        else
        {
            header = new SipTo(value);
        }
        break;
    case    SIP_TRANSFER_TO_HDR:
        if(value == "")
        {
            header = new SipTransferTo();
        }
        else
        {
            header = new SipTransferTo(value);
        }
        break;
    case    SIP_UNKNOWN_HDR:
        // right now, this does nothing, which is fine, but you can't
        // access such unknown headers -- they are merely passed
        // around silently
        break;
    case    SIP_UNSUPPORTED_HDR:
        if(value == "")
        {
            header = new SipUnsupported();
        }
        else
        {
            header = new SipUnsupported(value);
        }
        break;
    case    SIP_USER_AGENT_HDR:
        if(value == "")
        {
            header = new SipUserAgent();
        }
        else
        {
            header = new SipUserAgent(value);
        }
        break;
    case    SIP_VIA_HDR:
        if(value == "")
        {
            header = new SipVia();
        }
        else
        {
            header = new SipVia(value);
        }
        break;
    case    SIP_WARNING_HDR:
        if(value == "")
        {
            header = new SipWarning();
        }
        else
        {
            header = new SipWarning(value);
        }
        break;
    case    SIP_WWW_AUTHENTICATE_HDR:
        if(value == "")
        {
            header = new SipWwwAuthenticate();
        }
        else
        {
            header = new SipWwwAuthenticate(value);
        }
        break;
    case    SIP_SESSION_EXPIRES_HDR:
        if(value == "")
        {
            header = new SipSessionExpires();
        }
        else
        {
            header = new SipSessionExpires(value);
        }
        break;
    case    SIP_MIN_SE_HDR:
        if(value == "")
        {
            header = new SipMinSE();
        }
        else
        {
            header = new SipMinSE(value);
        }
        break;
        break;

    default:
	break;
    }
    return header;
}
