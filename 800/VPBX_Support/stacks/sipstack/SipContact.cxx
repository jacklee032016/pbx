/*
* $Log: SipContact.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipContact.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"
#include "symbols.hxx"
#include <cstdio>
#include "Data.hxx"
#include "cpLog.h"
#include "SipContact.hxx"
#include "SipParserMode.hxx"
#include "SipUrl.hxx"

using namespace Assist;

const SipContact::AllContactsTag SipContact::AllContacts = SipContact::AllContactsTag();


string
SipContactParserException::getName( void ) const
{
    return "SipContactParserException";
}




SipContact::SipContact(UrlType uType)
        : url(),
          qValue(""),
          expires(),
          bexpires(false),
          wildCard(false)

{
    urlType = uType;
}


SipContact::SipContact(const AllContactsTag & c)
    : url(),
      qValue(""),
      expires(),
      bexpires(false),
      wildCard(true)
{}
    


SipContact::SipContact( const Data& data )
        : url(),
        displayName(),
        qValue(""),
        expires(),
        bexpires(false),
          wildCard(false)
{
    try
    {
        decode(data);
	if (url.getPtr() != 0)
	{
	    urlType = url->getType();
	}
    }
    catch (SipContactParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "failed to decode the Contact string :o( ");
            throw SipContactParserException(
                "failed to decode the Contact string",
                __FILE__,
                __LINE__, DECODE_CONTACT_FAILED);
        }
    }
}

void
SipContact::decode( const Data& contactstr )
{
    if (contactstr == "*")
    {
        wildCard = true;
        return;
    }
    wildCard = false;
    try
    {
        parse(contactstr);
    }
    catch (SipContactParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "failed to decode the Contact string :o( ");
            throw SipContactParserException(
                "Parser Exception",
                __FILE__,
                __LINE__, DECODE_CONTACT_FAILED);
        }
    }
}

void
SipContact::parse( const Data & tmpdata)
{
    Data sipdata;
    Data data = tmpdata;
    int ret = data.match("<", &sipdata, true);
    if (ret == NOT_FOUND)
    {   //it can be URL?
        Data tmpval;
        Data value;
        // check  for the URL with Addrs-params
        int retn = data.match(";", &value, true) ;
        if (retn == NOT_FOUND)
        {  // it's Ok becos it Can be URl only since Addrs. Params are Optional
            tmpval = value;

            url = BaseUrl::decode(data);

        }
        else if (retn == FIRST)
        {
            if (SipParserMode::sipParserMode())
            {
                cpLog(LOG_ERR, "Mandatory item is not present :o( ");
                throw SipContactParserException(
                    "Mandatory item is not present",
                    __FILE__,
                    __LINE__, MANDATORY_ERR);
            }
        }
        else if (retn == FOUND)
        { // if found then it has the Addrs-params
            tmpval = value;

            url = BaseUrl::decode(tmpval);
            value = data;
            scanContactParam(value);
        }

    }
    else if (ret == FIRST)
    {
        Data sivalue;
        int rt = data.match(">", &sivalue, true) ;
        if (rt == NOT_FOUND)
        {  // it's Ok becos it Can be URl only since Addrs. Params are Optional
        }

        else if (rt == FIRST)
        {

            if (SipParserMode::sipParserMode())
            {
                cpLog(LOG_ERR, "Mandatory item is not present :o( ");
                throw SipContactParserException(
                    "Mandatory item is not present",
                    __FILE__,
                    __LINE__, MANDATORY_ERR);
            }

        }

        else if (rt == FOUND)
        { // if found then it has the Addrs-params
            Data tempval = sivalue;

            url = BaseUrl::decode(tempval);
            Data testdata = data;
            Data testvalue;
            int test = testdata.match(";", &testvalue, true);

            if (test == FOUND)
            {}

            else if (test == NOT_FOUND)
            {}

            else if (test == FIRST)
            {

                scanContactParam(testdata);
            }
        }
    }
    else if (ret == FOUND)
    {
        parseNameInfo(sipdata);

        Data sipdata1 = data;
        Data avalue;
        Data urlvalue;
        int check = sipdata1.match(">", &urlvalue, true);
        if (check == FOUND)
        {

            url = BaseUrl::decode(urlvalue);
            Data urlvalue1;
            int retrn = sipdata1.match(";", &urlvalue1, true);

            if (retrn == NOT_FOUND)
            {
                // In case Via parms are not there since *(via-parms)
                // is The Condition, this Checks for [Comment]
            }

            else if (retrn == FIRST)
            {
                scanContactParam(sipdata1);


            }
            else if (retrn == FOUND)
            {
                if (SipParserMode::sipParserMode())
                {
                    cpLog(LOG_ERR, "Mandatory item is not present :o( ");
                    throw SipContactParserException(
                        "Mandatory item is not present",
                        __FILE__,
                        __LINE__, MANDATORY_ERR);
                }




            }
        }
        else if (check == NOT_FOUND)
        {
        }

        else if ( check == FIRST)
        {
        }

    }

}


void
SipContact::parseUrl(const Data& condata)
{
    Data urlvalue = condata;
    Data avalue;
    int retur = urlvalue.match(">", &avalue, true);
    if (retur == NOT_FOUND)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Mandatory item > is not present :o( ");
            throw SipContactParserException(
                "Mandatory > item is not present",
                __FILE__,
                __LINE__, MANDATORY_ERR);
        }

    }
    else if (retur == FIRST)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Mandatory item > is not present :o( ");
            throw SipContactParserException(
                "Mandatory > item is not present",
                __FILE__,
                __LINE__, MANDATORY_ERR);
        }

    }
    else if (retur == FOUND)
    {

        url = BaseUrl::decode(avalue);

    }
}


void
SipContact::parseNameInfo(const Data& data)
{
    Data nameinfo = data;
    setDisplayName(nameinfo);
}


void
SipContact::scanContactParam(const Data& data)
{
    myParameterList.decode(data);
    // now, find and convert the expires header if we need it.

    SipParameterList::iterator i = myParameterList.find("expires");
    if(i != myParameterList.end())
    {
        expires.decode(i->second);
        myParameterList.erase(i);
        bexpires = true;
    }

    i = myParameterList.find("q");
    if(i != myParameterList.end())
    {
        setQValue(i->second);
        myParameterList.erase(i);
    }
}


bool 
SipContact::operator == (const SipContact& src) const
{
    if (isWildCard())
    {
        return src.isWildCard();
    }

    bool equal;
    equal = ( 
//        (displayName == src.displayName) &&
        (qValue == src.qValue) &&
        //(expires == src.expires) && // expires should not affect equality
        //(bexpires == src.bexpires) &&
        (myParameterList == src.myParameterList)
    );
    if (equal == false)
    {
	return equal;
    }
    //else compare urls.
    if ( (url.getPtr() == 0) && (src.url.getPtr() == 0))
    {
        equal = equal && true;
    }
    else if ( ( url.getPtr() != 0)  && (src.url.getPtr() != 0) )
    {
        equal = equal && ( url->areEqual(src.url));
    }
    else
    {
	equal = false; //since urls mismatch
    }
    return equal;

}

#define IsLess(a, b) if ((a) < (b)) return true; else if ((b) < (a)) return false

bool
SipContact::operator<(const SipContact& src) const
{
    if (isWildCard())
    {
        return !src.isWildCard();
    }
    
//    IsLess(displayName, src.displayName);
    IsLess(qValue, src.qValue);
    IsLess(myParameterList, src.myParameterList);
    
    //else compare urls.
    if ( (url.getPtr() == 0) && (src.url.getPtr() == 0))
    {
        return false;
    }
    else if ( ( url.getPtr() == 0) || (src.url.getPtr() == 0) )
    {
        return url.getPtr() == 0;
    }

    return url->isLessThan(src.url);
}
#undef IsLess

const SipContact&
SipContact::operator=(const SipContact& src)
{
    if ( &src != this)
    {
        url = duplicateUrl(src.url);
	urlType = src.urlType;
        displayName = src.displayName;
        qValue = src.qValue;
        expires = src.expires;
        bexpires = src.bexpires ;
        wildCard = src.wildCard;
        myParameterList = src.myParameterList;
    }
    return (*this);
}

void
SipContact::setNullContact(){
    wildCard = true;
}

SipContact::SipContact( const SipContact& src )
    : 
    url(duplicateUrl(src.url)),
    urlType(src.urlType),
    myParameterList(src.myParameterList)
{
    //assign individual members.

    setDisplayName(src.displayName);
    setQValue(src.qValue);
    setExpires(src.expires);
    bexpires = src.bexpires;
    wildCard = src.wildCard;
}


Data 
SipContact::encode() const
{
    Data data = CONTACT;
    data += SP;
    if (isWildCard())
    {
        data += "*";
        data += CRLF;
        return data;
    }

    data += displayName ;
        
    // always use anglebrackets for encoding, as this simplifies things
    // and avoids the nasty ambiguity in whether parameters are
    // for the URL or the Contact field.
    bool useAngleBrackets = true;
        
    if (url.getPtr() != 0)
    {
        if(useAngleBrackets)
        {
	    data += "<";
        }

	url->encode();
	if (url->getType() == SIP_URL)
	{
	    Sptr <SipUrl> sipUrl;
	    sipUrl.dynamicCast(url);
    

	    Data nameaddr;
	    
	    nameaddr = sipUrl->getNameAddr();
	    
	    data += nameaddr;
	    
	    Data urlparam;
	    urlparam = sipUrl->getUrlParam();
	    if (urlparam.length())
	    {
		data += urlparam;
	    }
	}
        else
        {
            // just output the URL, no brackets
            data += url->encode();
        }
            
        if(useAngleBrackets)
        {
            data += ">";
        }
            
    }
        
    // encode via the parameter list
    Data pData = myParameterList.encode();
    if(pData.length())
    {
        data += ";";
        data += myParameterList.encode();
    }
        
    if (bexpires)
    {
        data += ";";
        data += "expires=";
        data += expires.getData();
    }
    
    if (qValue != "")
    {
        data += ";";
        data += "q=";        
        data += qValue;
    }                

    data += CRLF;
    return data;
}


Sptr <BaseUrl>
SipContact::getUrl() const
{
    return duplicateUrl(url, true);
}


void
SipContact::setUrl( Sptr <BaseUrl>  newurl )
{
    url = duplicateUrl(newurl);
}


Data
SipContact::getDisplayName() const
{
    return displayName;
}


void
SipContact::setDisplayName( const Data& newdisplayName )
{
    displayName = newdisplayName;
}

bool 
SipContact::isWildCard() const
{
    return wildCard;
}

Data
SipContact::getQValue() const
{
    return qValue;
}


void
SipContact::setQValue( const Data& newqValue )
{
    qValue = newqValue;
}

SipExpires
SipContact::getExpires() const
{
    return expires;
}

void
SipContact::setExpires( const SipExpires& newexpires )
{
    SipExpires fexpire(newexpires);
    expires = fexpire;
    bexpires = true;
}


SipHeader*
SipContact::duplicate() const
{
    return new SipContact(*this);
}


bool
SipContact::compareSipHeader(SipHeader* msg) const
{
    SipContact* otherMsg = dynamic_cast<SipContact*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}

ostream&
Assist::operator<<(ostream& s, const SipContact &msg)
{
    s << msg.encode();
    return s;
}
