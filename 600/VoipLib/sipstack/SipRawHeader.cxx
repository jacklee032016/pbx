/*
 * $Id: SipRawHeader.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */

#include "global.h"
#include "SipRawHeader.hxx"
#include "cpLog.h"

using namespace Vocal;

bool 
SipRawHeader::shallowEquals(const SipRawHeader& src) const
{
    if((src.header == 0) && (header == 0))
    {
        return ((headerType == src.headerType) &&
                (headerValue == src.headerValue) &&
                ( (headerType != SIP_UNKNOWN_HDR) ||
                  (headerName == src.headerName)));
    }
    else
    {
        // at least one is parsed -- parse both and go for it
        src.parse();
        parse();
        return (header->compareSipHeader(src.header.getPtr()));
    }
}


bool 
SipRawHeader::shallowEqualsNul() const
{
    // a header is equal to the NUL pointer if 
    if(header == 0)
    {
        // in this case, something is going to get encoded, so it
        // can't possibly equal the NUL pointer.
        cpDebug(LOG_DEBUG_STACK, "no header so returning false");
        return false;
    }
    else if ((headerType == SIP_CONTENT_TYPE_HDR) || 
             (headerType == SIP_CONTENT_LENGTH_HDR) ||
             (headerType == SIP_CONTENT_DISPOSITION_HDR) ||
             (headerType == SIP_CONTENT_ENCODING_HDR) ||
             (headerType == SIP_CONTENT_LANGUAGE_HDR) ||
             (headerType == SIP_MAX_FORWARDS_HDR)
        )
    {
        // for purposes of comparison w/ a null header, these always
        // are "true" (because they may not yet have been set
        // correctly)
        cpDebug(LOG_DEBUG_STACK, "content header so returning true");
        return true;
    }
    else // compare for real
    {
        // at least one is parsed, so there is a chance that this is
        // the right one
        Data encoding;
        encoding = header->encode();
        cpDebug(LOG_DEBUG_STACK, "encoded header: \"%s\"", encoding.logData());
        return (encoding == "");
    }
}


bool
SipRawHeader::operator==(const SipRawHeader& src) const
{
    // need to figure this one out
    bool compare = shallowEquals(src);

    if(compare)
    {
        Sptr<SipRawHeader> left = next;
        Sptr<SipRawHeader> right = src.next;

        while( compare && ((left != 0) && (right != 0)))
        {
            // keep comparing children
            compare = left->shallowEquals(*right);
            left = left->next;
            right = right->next;
        }
        if( compare && (left == 0) && (right == 0))
        {
            // in this case, both ended at the same time, so if
            // compare is true, both are "equal"
            compare = true;
        }
        else
        {
            // otherwise, false
            compare = false; 
        }
    }

    return compare;
}


bool
SipRawHeader::operator!=(const SipRawHeader& src) const
{
    return !(*this == src);
}


static void encodeShallowPutsName(const SipRawHeader& header, Data* msg)
{
    if (header.header != 0)
    {
        // call the parsed header encode
        *msg += header.header->encode();
    }
    else
    {
        // no parsed header, reconstruct
        if(header.headerType != SIP_UNKNOWN_HDR)
        {
            *msg += headerTypeEncode(header.headerType);
        }
        else
        {
            *msg += header.headerNameOriginal;
        }
        *msg += ": ";
        Data headerVal = header.headerValue;
        if (headerVal.length())
        {
            *msg += header.headerValue;
        }
        *msg += "\r\n";
    }
}


static void encodeShallowNoName(const SipRawHeader& header, Data* msg, bool comma)
{
    if(header.header != 0)
    {
        if(comma)
        {
            *msg += ",";
        }
        *msg += header.header->encode();
    }
    else if(header.headerValue.length() != 0)
    {
        if(comma)
        {
            *msg += ",";
        }
        *msg += header.headerValue;
    }
}


bool
SipRawHeader::encode(Data* msg) const
{
    switch(headerType)
    {
    case SIP_ACCEPT_HDR:
    case SIP_ACCEPT_ENCODING_HDR:
    case SIP_ACCEPT_LANGUAGE_HDR:
    case SIP_CONTENT_ENCODING_HDR:
    case SIP_RECORD_ROUTE_HDR:
    case SIP_SUPPORTED_HDR:
        
    case SIP_CONTENT_LANGUAGE_HDR:
    case SIP_ALLOW_HDR:
        
    case SIP_PROXY_REQUIRE_HDR:
    case SIP_REQUIRE_HDR:
    case SIP_ROUTE_HDR:
        // in this case, this is a list -- encode that way!
        *msg += headerTypeEncode(headerType);
        *msg += ": ";
        encodeShallowNoName(*this, msg, false);
        // keep encoding if this is a list
        {
            Sptr<SipRawHeader> tmp = next;
            while(tmp != 0)
            {
                encodeShallowNoName(*tmp, msg, true);
                tmp = tmp->next;
            }
            *msg += "\r\n";
        }
        break;
    default:
        encodeShallowPutsName(*this, msg);
        // keep encoding if this is a list
        {
            Sptr<SipRawHeader> tmp = next;
            while(tmp != 0)
            {
                encodeShallowPutsName(*tmp, msg);
                tmp = tmp->next;
            }
        }
        break;
    }
    
    return false;
}


bool
SipRawHeader::decode(const Data& headerData)
{
    bool parseError = false;
    bool noMatch;

    // decode the next line of the message
    headerValue = headerData;
    headerName = headerValue.parse(": \t", &noMatch);
    headerNameOriginal = headerName;
    headerName.lowercase();
    headerType = headerTypeDecode(headerName);
    if(noMatch)
    {
	// some sort of error
	parseError = true;
    }

    // check for possible multiple headers

    switch(headerType)
    {
        // all of these are multiple headers, so do the multiple
        // header case for them.

    case SIP_VIA_HDR:
    case SIP_DIVERSION_HDR:
    case SIP_ACCEPT_HDR:
    case SIP_ACCEPT_ENCODING_HDR:
    case SIP_ACCEPT_LANGUAGE_HDR:
    case SIP_RECORD_ROUTE_HDR:
    case SIP_SUPPORTED_HDR:
    case SIP_CONTENT_LANGUAGE_HDR:
    case SIP_ALLOW_HDR:
    case SIP_CONTENT_ENCODING_HDR:
    case SIP_PROXY_REQUIRE_HDR:
    case SIP_REQUIRE_HDR:
    case SIP_ROUTE_HDR:
        
    case SIP_UNSUPPORTED_HDR:
    case SIP_IN_REPLY_TO_HDR:
    case SIP_WARNING_HDR:
    case SIP_CONTACT_HDR:
    {
        // multiple header parsing
        SipRawHeader* current = this;
        bool notFound;

        do
        {
            Data tmp = current->headerValue;
            Data found = tmp.parseOutsideQuotes(",", true, true, &notFound);
            if(!notFound)
            {
                if(found != "")
                {
                    // found, do something
                    current->headerValue = found;
                    current->next = new SipRawHeader;
                    
                    current->next->headerValue = tmp;
                    current->next->headerValue.removeSpaces();
                    current->next->headerName = headerName;
                    current->next->headerNameOriginal = headerNameOriginal;
                    current->next->headerType = headerType;
                    current = current->next.getPtr();
                }
                else
                {
                    // there is a problem here, in that there was no
                    // text prior to the comma, e.g

                    // Route : ,<sip:tisong@1.1.1.1>

                    current->headerValue = tmp;
                    cpDebug(LOG_DEBUG_STACK, "empty header separated by comma");
                }
            }
        }
        while(!notFound);
    }
    break;
    default:
        // do nothing here for single headers 
        break;
    }


    return parseError;
}


bool
SipRawHeader::parse() const
{
    if(header == 0)
    {
        header = SipHeader::decodeHeader(headerType, headerValue);
    }
    return true;
}


bool
SipRawHeader::parseDeep() const
{
    const SipRawHeader* tmp;
    tmp = this;
    while(tmp != 0)
    {
        tmp->parse();
        tmp = tmp->next.getPtr();
    }

    return true;
}


void
SipRawHeader::deepCopy(const SipRawHeader& src)
{
    assert(0);
    *this = src; // shallow copy
    if(src.header != 0)
    {
        this->header = src.header->duplicate();
    }
//    this->next = deepCopy
    if(src.next != 0)
    {
        this->next = src.next->duplicate();
    }
}


SipRawHeader*
SipRawHeader::duplicate() const
{
    SipRawHeader* tmp = new SipRawHeader(*this); // make a shallow copy
    if(this->header != 0)
    {
        tmp->header = this->header->duplicate();
    }
    if(this->next != 0)
    {
        tmp->next = this->next->duplicate();
    }
    return tmp;
}
