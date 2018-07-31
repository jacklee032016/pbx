#ifndef SIP_HEADER_HXX_
#define SIP_HEADER_HXX_
/*
* $Log: SipHeader.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipHeader.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "Data.hxx"
#include "Sptr.hxx"
#include "VException.hxx"
#include <deque>

/*
  #ifdef __linux__
  // hash_map is much faster than regular map
  #include <hash_map>
  #include "StringHash.hxx"
  typedef int SipHeaderType;
  typedef hash_map<SipHeaderType, pair<Data,Data>, StringHash > SipHeaderNameMap;
  #else
  // Some STL implementations do not support the hash map eg. Solaris
  // so use map instead.  Note: map is much slower
  #include <map>
  typedef int SipHeaderType;
  typedef map<SipHeaderType, pair<Data,Data>> SipHeaderNameMap;
  #endif
*/


namespace Assist
{


//Exception handling for all headers of the message 
enum SipHeaderErrorType
{
    CSEQ_OUT_OF_RANGE,
    VIA_NOT_PRESENT,
    INDEX_OUT_OF_BOUND,
    TTL_OUT_OF_RANGE,
    OUT_OF_RANGE
};

///class to handle exceptions of SipHeaders.
class SipHeaderException
{
    public:
        SipHeaderException(SipHeaderErrorType i) { value = i; }
        SipHeaderErrorType value;
};

/** enum of all SipHeader types.  

* If you add another Header type, you must:<p>
* <ol>
* <li> change the table of text encodings for this enum in
* sipHeaderTable (in SipHeader.cxx)
*
* <li> add it to SipHeader::decodeHeader()
*
* <li> add the header to the sipHeaderOutputOrder in
* SipRawHeaderContainer.cxx
* </ol>
* please keep this in alphabetical order for cleanliness.
* <p>
* <b>remember that SIP_UNKNOWN_HDR must be last.</b>
*
*/
enum SipHeaderType
{
    SIP_ACCEPT_HDR,
    SIP_ACCEPT_ENCODING_HDR,
    SIP_ACCEPT_LANGUAGE_HDR,
    SIP_ALLOW_HDR,
    SIP_ALSO_HDR,
    SIP_AUTHORIZATION_HDR,

    SIP_CALLID_HDR,
    SIP_CALL_LEG_HDR,
    SIP_CONTACT_HDR,
    SIP_CONTENT_DATA_HDR,
    SIP_CONTENT_DISPOSITION_HDR,
    SIP_CONTENT_ENCODING_HDR,
    SIP_CONTENT_LANGUAGE_HDR,
    SIP_CONTENT_LENGTH_HDR,
    SIP_CONTENT_TYPE_HDR,
    SIP_CSEQ_HDR,

    SIP_DATE_HDR,
    SIP_DIVERSION_HDR,

    SIP_ENCRYPTION_HDR,
    SIP_EXPIRES_HDR,
    SIP_EVENT_HDR,

    SIP_FROM_HDR,

    SIP_HIDE_HDR,

    SIP_IN_REPLY_TO_HDR,

    SIP_MAX_FORWARDS_HDR,
    SIP_MIME_VERSION_HDR,

    SIP_ORGANIZATION_HDR,
    SIP_OSP_AUTHORIZATION_HDR,

    SIP_PRIORITY_HDR,
    SIP_PROXY_AUTHENTICATE_HDR,
    SIP_PROXY_AUTHORIZATION_HDR,
    SIP_PROXY_REQUIRE_HDR,

    SIP_RECORD_ROUTE_HDR,
    SIP_REFERRED_BY_HDR,
    SIP_REFER_TO_HDR,
    SIP_REPLACES_HDR,
    SIP_REQUIRE_HDR,
    SIP_RESPONSE_KEY_HDR,
    SIP_RETRY_AFTER_HDR,
    SIP_ROUTE_HDR,

    SIP_SERVER_HDR,
    SIP_SESSION_HDR,
    SIP_SUBJECT_HDR,
    SIP_SUBSCRIPTION_NOTIFY_HDR,
    SIP_SUPPORTED_HDR,

    SIP_TIMESTAMP_HDR,
    SIP_TO_HDR,
    SIP_TRANSFER_TO_HDR,

    SIP_UNSUPPORTED_HDR,
    SIP_USER_AGENT_HDR,

    SIP_VIA_HDR,

    SIP_WARNING_HDR,
    SIP_WWW_AUTHENTICATE_HDR,

    SIP_SESSION_EXPIRES_HDR,
    SIP_MIN_SE_HDR,

    SIP_UNKNOWN_HDR // this header needs to be kept last -- see
    // sipHeaderNameCount
};

/// this is the number of headers in the SipHeaderType enum.
const int sipHeaderNameCount = SIP_UNKNOWN_HDR + 1;

///
SipHeaderType headerTypeDecode(const Data& headerName);
///
Data headerTypeEncode(const SipHeaderType header);


/// pure virtual base class for all SIP headers
class SipHeader
{
    public:
        static  void    init();
        /// virtual destructor
        virtual ~SipHeader() {};
        /// encode function returns the encoded form of the header
        virtual Data encode() const = 0;
        /// factory for constructing sip headers given the type
        static SipHeader* decodeHeader( SipHeaderType type, const Data& value );
        /// virtual method for copying sip headers of any type without knowing which type
        virtual SipHeader* duplicate() const = 0;
        /// compare two headers of (possibly) the same class
        virtual bool compareSipHeader(SipHeader* msg) const = 0;

        ///
        bool isEmpty() const { return emptyFlg; };

    protected:
        SipHeader() { emptyFlg= true; }
        SipHeader(const Data& val) { emptyFlg= false; }
        bool emptyFlg;
};
   
}

#endif
