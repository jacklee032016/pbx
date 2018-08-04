/*
* $Log: SipRawHeaderContainer.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipRawHeaderContainer.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"
#include "SipRawHeader.hxx"
#include "SipRawHeaderContainer.hxx"
#include "cpLog.h"

using namespace Assist;

// This struct specifies the order in which the headers will be output
// when encode() is called.  The headers which appear first in the
// sipHeaderOutputOrder will appear first, while later ones will
// appear later.  note that sipHeaderOutputOrder must contain all of
// the headers to be output -- if the header does not appear in this
// struct, it will not be encoded.

const SipHeaderType sipHeaderOutputOrder[] =
{
    SIP_VIA_HDR,
    SIP_TO_HDR,
    SIP_FROM_HDR,
    SIP_DATE_HDR,

    SIP_TRANSFER_TO_HDR,
    SIP_CALLID_HDR,
    SIP_CSEQ_HDR,

    SIP_PRIORITY_HDR,
    SIP_PROXY_AUTHORIZATION_HDR,
    SIP_OSP_AUTHORIZATION_HDR,


    SIP_MAX_FORWARDS_HDR,
    SIP_ORGANIZATION_HDR,
    SIP_SUBJECT_HDR,
    SIP_HIDE_HDR,

    SIP_EXPIRES_HDR,
    SIP_RESPONSE_KEY_HDR,
    SIP_REFER_TO_HDR,
    SIP_REFERRED_BY_HDR,
    SIP_REPLACES_HDR,

    SIP_SERVER_HDR,
    SIP_ROUTE_HDR,
    SIP_REQUIRE_HDR,
    SIP_RECORD_ROUTE_HDR,
    SIP_CONTACT_HDR,




    SIP_ACCEPT_HDR,
    SIP_ACCEPT_ENCODING_HDR,
    SIP_ACCEPT_LANGUAGE_HDR,

    SIP_ALLOW_HDR,
    SIP_ALSO_HDR,
    SIP_AUTHORIZATION_HDR,

    SIP_CALL_LEG_HDR,

    SIP_CONTENT_DATA_HDR,
    SIP_CONTENT_DISPOSITION_HDR,
    SIP_CONTENT_ENCODING_HDR,
    SIP_CONTENT_LANGUAGE_HDR,

    SIP_DIVERSION_HDR,

    SIP_ENCRYPTION_HDR,



    SIP_IN_REPLY_TO_HDR,

    SIP_MIME_VERSION_HDR,


    SIP_PROXY_AUTHENTICATE_HDR,
    SIP_PROXY_REQUIRE_HDR,



    SIP_RETRY_AFTER_HDR,

    SIP_SESSION_HDR,
    SIP_SUBSCRIPTION_NOTIFY_HDR,
    SIP_SUPPORTED_HDR,

    SIP_TIMESTAMP_HDR,

    SIP_UNSUPPORTED_HDR,
    SIP_USER_AGENT_HDR,

 
    SIP_WARNING_HDR,
    SIP_WWW_AUTHENTICATE_HDR,
    SIP_EVENT_HDR,

    SIP_SESSION_EXPIRES_HDR,
    SIP_MIN_SE_HDR,

    SIP_CONTENT_TYPE_HDR,
    SIP_CONTENT_LENGTH_HDR,

    SIP_UNKNOWN_HDR
};


SipRawHeaderContainer::SipRawHeaderContainer()
    :
    vector<Sptr<SipRawHeader> >(sipHeaderNameCount)
{
}


SipRawHeaderContainer::SipRawHeaderContainer(const SipRawHeaderContainer& src)
    :
    vector<Sptr<SipRawHeader> >(sipHeaderNameCount)
{
    SipRawHeaderContainer::const_iterator i;
    SipRawHeaderContainer::iterator j = this->begin();

    for(i = src.begin(); i != src.end(); ++i)
    {
        if((*i) != 0)
        {
            (*j) = (*i)->duplicate();
        }
        ++j;
    }
}


SipRawHeaderContainer::~SipRawHeaderContainer()
{
}


const SipRawHeaderContainer&
SipRawHeaderContainer::operator=(const SipRawHeaderContainer& src)
{
    if(&src != this)
    {
        SipRawHeaderContainer::const_iterator i;
        SipRawHeaderContainer::iterator j = this->begin();
        
        for(i = src.begin(); i != src.end(); ++i)
        {
            if((*i) != 0)
            {
                (*j) = (*i)->duplicate();
            }
            ++j;
        }
    }
    return *this;
}


bool
SipRawHeaderContainer::operator==(const SipRawHeaderContainer& src) const
{
    if(&src == this)
    {
        cpDebug(LOG_DEBUG_STACK, "SipRawHeaderContainer::operator== returns true");
        return true;
    }
    else
    {
        bool compare = true;

        int count = 0;
        SipRawHeaderContainer::const_iterator i;
        SipRawHeaderContainer::const_iterator j = this->begin();
        
        for(i = src.begin(); i != src.end(); ++i)
        {
            if( ((*i) != 0) && ((*j) != 0))
            {
                // compare using SipRawHeader's operator== 
                bool tmp = (*(*j) == *(*i));
                cpDebug(LOG_DEBUG_STACK, 
                      "SipRawHeaderContainer::operator== returns %s (%d)",
                      (tmp ? "true" : "false"),
                      count);
                
                compare = compare && tmp;
            }
            else
            {
                bool tmp;
                if((*i) != 0)
                {
                    tmp = (*i)->shallowEqualsNul();
                }
                else if ((*j) != 0)
                {
                    tmp = (*j)->shallowEqualsNul();
                }
                else
                {
                    // both are empty
                    tmp = true;
                }
                cpDebug(LOG_DEBUG_STACK, 
                      "SipRawHeaderContainer::operator== returns %s (%d)",
                      (tmp ? "true" : "false"),
                      count);
                compare = compare && tmp;
            }
            ++j;
            ++count;
        }
        return compare;
    }
}


bool
SipRawHeaderContainer::operator!=(const SipRawHeaderContainer& src) const
{
    return !(*this == src);
}


void
SipRawHeaderContainer::insertManyHeader(Sptr<SipRawHeader> header)
{
    assert((header->headerType >= 0) && (header->headerType < sipHeaderNameCount));

    // todo: this could verify which headers can and cannot have multiple
    // instances

    // insert the value at the end of the linked list of SipRawHeaders
    // this is necessary to preserve the order of headers like Via.

    Sptr<SipRawHeader>* insertLocation = 
	&(this->operator[](header->headerType));

    while((*insertLocation) != 0)
    {
	insertLocation = &((*insertLocation)->next);
    }

    *insertLocation = header;
}


void
SipRawHeaderContainer::insertHeader(Sptr<SipRawHeader> header, int pos)
{
    assert((header->headerType >= 0) && (header->headerType < sipHeaderNameCount));

    // todo: this could verify which headers can and cannot have multiple
    // instances

    Sptr<SipRawHeader>* insertLocation = 
	&(this->operator[](header->headerType));

    while(((*insertLocation) != 0) && (pos != 0))
    {
	insertLocation = &((*insertLocation)->next);
        if(pos > 0) 
        {
            pos--;
        }
    }

    if((pos == 0) || (pos == -1))
    {
        // good
        header->next = *insertLocation;
        *insertLocation = header;
    }
    else
    {
        // this is a problem -- if pos > 0 -- that means that it has
        // hit the last element and so the user has indexed off the
        // end.  assert here
        cpLog(LOG_ALERT, "index to insert item not within scope");
        assert(0);
    }
}

Sptr<SipRawHeader>
SipRawHeaderContainer::getHeader(SipHeaderType type, int index/* Default value=0*/) const
{
    assert((type >= 0) && (type < sipHeaderNameCount));
    Sptr<SipRawHeader> p = this->operator[](type);
    while(index)
    {
        assert(p != 0);
        if(p != 0)
        {
            p = p->next;
        }
        index--;
    }

    return p;
}


void
SipRawHeaderContainer::replaceHeader(SipHeaderType type, Sptr<SipRawHeader> header)
{
    this->operator[](type) = header;
}


Sptr<SipRawHeader> 
SipRawHeaderContainer::getHeaderOrCreate(SipHeaderType type, int index /*Default value=0 */) const
{

    assert((type >= 0) && (type < sipHeaderNameCount));
    Sptr<SipRawHeader>* p 
        = const_cast< Sptr<SipRawHeader>* >(&(this->operator[](type)));
    Sptr<SipRawHeader> q = 0;

    if(index >= 0)
    {
        while(index)
        {
            if((*p) == 0)
            {
                // create an empty entry...
                *p = new SipRawHeader;
                (*p)->headerType = type;
            }
            p = &((*p)->next);
            index--;
        }
        if((*p) == 0)
        {
            // create an empty entry...
            *p = new SipRawHeader;
            (*p)->headerType = type;
        }
        return *p;
    }
    else
    {
        /// make sure this is right
        if((*p) == 0)
        {
            // create an empty entry...
            *p = new SipRawHeader;
            (*p)->headerType = type;
        }
        // presuming to get the last element
        while((*p) != 0)
        {
            q = *p;
            p = &((*p)->next);
        }

        return q;
    }
    return 0;
}


bool
SipRawHeaderContainer::decode(Data* headers)
{
    bool parseError = false;

    for(;;)
    {
	bool noMatch = false;

	Sptr<SipRawHeader> oneHeader = new SipRawHeader;

	Data oneLine = headers->getLine(&noMatch);

        cpDebug(LOG_DEBUG_STACK, "parsingline: %s", oneLine.logData());

	if(noMatch)
	{
            // did not find another line, so we're done
	    break;
	}

	// otherwise, we have one line.  check to see if its empty. if
	// it is, we're done

	if(oneLine == "")
	{
	    break;
	}

	if(oneHeader->decode(oneLine))
	{
	    // some sort of error
	    parseError = true;
	    break;
	}

	// otherwise, we're ok so put the key / value pair in the deque

        insertManyHeader(oneHeader);
    }

    return parseError;
}


bool
SipRawHeaderContainer::encode(Data* msg) const
{
    bool error = false;
    for(int i = 0; i < sipHeaderNameCount; i++)
    {
	Sptr<SipRawHeader> header = this->operator[](sipHeaderOutputOrder[i]);

        if(header != 0)
        {
            if(header->encode(msg))
            {
                error = true;
                break;
            }
        }
    }
    return error;
}


void
SipRawHeaderContainer::mask( set <SipHeaderType>& mask )
{
    set<SipHeaderType>::iterator i;
    i = mask.begin();
    while(i != mask.end())
    {
        this->operator[](*i) = 0; // erase
        i++;
    }
}


int SipRawHeaderContainer::getNumHeaders( SipHeaderType type ) const
{
    int i = 0;
    Sptr<SipRawHeader> p = operator[](type);

    while(p != 0)
    {
        i++;
        p = p->next;
    }

    return i;
}


void SipRawHeaderContainer::removeHeader ( SipHeaderType type, 
                                           int index /* default value */ )
{
    assert((type >= 0) && (type < sipHeaderNameCount));
    Sptr<SipRawHeader>* p = &(this->operator[](type));
    Sptr<SipRawHeader> q;

    if(index != -1)
    {
        while(index && (*p != 0))
        {
            p = &((*p)->next);
            index--;
        }
        if(index)
        {
            // ran out of elements before my time
            // this is a problem -- if pos > 0 -- that means that it has
            // hit the last element and so the user has indexed off the
            // end.  assert here
            cpLog(LOG_ALERT, "index to remove item not within scope");
            assert(0);
        }
        else
        {
            if((*p) != 0)
            {
                q = (*p)->next;
                *p = q;
            }
        }
    }
    else
    {
        while((*p != 0) && ((*p)->next != 0))
        {
            p = &((*p)->next);
        }
        *p = 0;
    }

}


void SipRawHeaderContainer::setNumHeaders ( SipHeaderType type, int number )
{
    assert((type >= 0) && (type < sipHeaderNameCount));
    assert(number >= 0);

    Sptr<SipRawHeader>* p = &(this->operator[](type));
    while(number && (*p != 0))
    {
        p = &((*p)->next);
        number--;
    }
    *p = 0;
}


void SipRawHeaderContainer::appendHeader ( SipHeaderType type, 
                                           const Sptr<SipHeader> p,
                                           int pos /* default argument */)
{
    // create a scratch header
    Sptr<SipRawHeader> oneHeader = new SipRawHeader;

    oneHeader->header = p;
    oneHeader->headerType = type;

    insertHeader(oneHeader, pos);
}


void 
SipRawHeaderContainer::forceParse()
{
    SipRawHeaderContainer::iterator i;
    for(i = this->begin(); i != this->end(); ++i)
    {
        if ((*i) != 0)
        {
            (*i)->parseDeep();
        }
    }
}
