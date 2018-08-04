/*
* $Log: SipReferredBy.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:10  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipReferredBy.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include "SipReferredBy.hxx"
#include "symbols.hxx"
#include "SipParserMode.hxx"
#include "BaseUrl.hxx"
#include "SipUrl.hxx"
#include "TelUrl.hxx"
#include "Sptr.hxx"

using namespace Assist;

string
SipReferredByParserException::getName( void ) const
{
    return "SipReferredByParserException";
}


SipReferredBy::SipReferredBy(UrlType uType)
    :
    referrerUrl(),
    referencedUrl(),
    displayName(),
    address(),
    pgpMap(),
    authScheme("pgp"),
    //   basicCookie(),
    tokenMutex()

{
}


SipReferredBy::~SipReferredBy()
{
}


SipReferredBy::SipReferredBy(const SipReferredBy& src)
        :
        referrerUrl(duplicateUrl(src.referrerUrl)),
	urlType(src.urlType),
        referencedUrl(src.referencedUrl),
        displayName(src.displayName),
        address(src.address),
        pgpMap(src.pgpMap),
        authScheme(src.authScheme)
        //basicCookie(src.basicCookie)
{
}


SipReferredBy::SipReferredBy( const Data& data )
    :
    referrerUrl(),
    referencedUrl(),
    displayName(),
    address(),
    pgpMap(),
    authScheme("pgp"),
    //basicCookie(),
    tokenMutex()
{
    try
    {
        decode(data);
	if (referrerUrl.getPtr() != 0)
	{
	    urlType = referrerUrl->getType();
	}
    }
    catch (SipReferredByParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decodein Constructor :o( ");
            throw SipReferredByParserException("failed in Decode", __FILE__, __LINE__, DECODE_REFERREDBY_FAILED);

        }

    }
}


void
SipReferredBy::scanReferrerUrl( const Data & tmpdata)
{
    Data data = tmpdata;
    Data sipdata;
    int ret = data.match("<", &sipdata, true);
    if (ret == NOT_FOUND)
    {
        Data value;
        Data tmpval;
        int retn = data.match("\n", &value, true) ;
        if (retn == NOT_FOUND)
        {
	    // AmyS - 05/16/02 Decode the BaseUrl here.
	    tmpval = value;
	    referrerUrl = BaseUrl::decode(data);
/*
            if (SipParserMode::sipParserMode())
            {
                cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");
                throw SipReferredByParserException("failed in Decode", __FILE__, __LINE__, DECODE_REFERREDBY_FAILED);

             } 
*/
        }
        else if (retn == FIRST)
        {
            if (SipParserMode::sipParserMode())
            {
                cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");
                throw SipReferredByParserException("failed in Decode", __FILE__, __LINE__, DECODE_REFERREDBY_FAILED);

            }

        }
        else if (retn == FOUND)
        {
            tmpval = value;
            Data refdata;
            int retval = tmpval.match(SP, &refdata, true);
            if ( retval == FOUND)
            {
                referrerUrl = BaseUrl::decode(tmpval);
                scanReferencedUrl(refdata);
            }
        }

    }
    else if (ret == FIRST)
    {
        try
        {
            parseUrl(data);
        }
        catch (SipReferredByParserException&)
        {
            if (SipParserMode::sipParserMode())
            {
                cpLog(LOG_ERR, "Failed to Decode in Scan SIPTO :o( ");
                throw SipReferredByParserException("failed in Decode in scan SipReferredBy", __FILE__, __LINE__, PARSE_REFERRERURL_FAILED);

            }

        }

    }

    else if (ret == FOUND)
    {
        parseNameInfo(sipdata);
        Data urlvalue;
        parseUrl(data);
    }
}


void
SipReferredBy::scanReferencedUrl( const Data & tmpdata)
{
    Data sipdata;
    Data data;
    Data sdata = tmpdata;

    int ret = sdata.match(":", &sipdata, true);
    if (ret == NOT_FOUND)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");
            throw SipReferredByParserException("failed in Decode", __FILE__, __LINE__, DECODE_REFERREDBY_FAILED);

        }

    }
    else if (ret == FOUND)
    {
        Data urldata;
        referencedUrl = sipdata;
        int retn = sdata.match("//", &urldata, true);
        if (retn == FIRST)
        {
            Data urData;
            int retl = sdata.match(CRLF, &urData, true);
            if ( retl == FOUND)
            {
                urData.removeSpaces();
                address = urData;
            }
            else if ( retl == NOT_FOUND)
            {
                sdata.removeSpaces();
                address = sdata;
            }
        }
        else
        {
            sipdata.removeSpaces();

            if ( sipdata == "sip")
            {
                Data urData;
                int retl = sdata.match(CRLF, &urData, true);
                if ( retl == FOUND)
                {
                    address = urData;
                }
                else if ( retl == NOT_FOUND)
                {
                    address = sdata;
                }
            }
            else
            {
                if (SipParserMode::sipParserMode())
                {
                    cpLog(LOG_ERR, "Failed to Decode in Scan SIPTO :o( ");
                    throw SipReferredByParserException("failed in Decode in scan SipReferredBy", __FILE__, __LINE__, PARSE_REFERENCEDURL_FAILED);

                }
            }
        }
    }
    else if (ret == FIRST)
    {

        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Scan SIPTO :o( ");
            throw SipReferredByParserException("failed in Decode in scan SipReferredBy", __FILE__, __LINE__, PARSE_REFERENCEDURL_FAILED);

        }



    }
}


void
SipReferredBy::parse( const Data& refertodata)
{

    Data data = refertodata;

    try
    {
        Data tempdata;
        int ret = data.match("pgp", &tempdata, true);
        if ( ret == FIRST)
        { // exception
            if (SipParserMode::sipParserMode())
            {
                cpLog(LOG_ERR, "Failed to Decode in Parse :o( ");
                throw SipReferredByParserException("failed in Decode", __FILE__, __LINE__, DECODE_REFERREDBY_FAILED);

            }

        }

        if ( ret == FOUND)
        {
            scanAuthTokens(data);
            scanReferrerUrl(tempdata);
        }
        if ( ret == NOT_FOUND)
        {
            scanReferrerUrl(data);
        }

    }
    catch (SipReferredByParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Parse :o( ");
            throw SipReferredByParserException("failed in Decode", __FILE__, __LINE__, DECODE_REFERREDBY_FAILED);

        }

    }


}




void
SipReferredBy::parseUrl(const Data& data)
{
    Data urlvalue = data;
    Data avalue;
    int retur = urlvalue.match(">", &avalue, true);
    if (retur == NOT_FOUND)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Parse in ParseUrl() :o( ");
            throw SipReferredByParserException("failed in Decode", __FILE__, __LINE__, PARSE_REFERRERURL_FAILED);

        }


    }
    else if (retur == FIRST)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Parse in ParseUrl()  :o( ");
            throw SipReferredByParserException("failed in Decode", __FILE__, __LINE__, PARSE_REFERRERURL_FAILED);

        }

    }
    else if (retur == FOUND)
    {
        referrerUrl = BaseUrl::decode(avalue);
        scanReferencedUrl(avalue);
    }
}






void
SipReferredBy::parseNameInfo(const Data& data)
{
    Data newnameinfo;
    Data nameinfo = data;
    int ret = nameinfo.match(":", &newnameinfo, true);
    Data newnameinf;
    ret = nameinfo.match(" ", &newnameinf, true);
    setDisplayName(nameinfo);
}



void SipReferredBy::setDisplayName(const Data& name)
{
    displayName = name;
}


void
SipReferredBy::decode( const Data& alsostr )
{

    try
    {

        Data palsostr = REFERRED_BY;
        palsostr += ":";
        palsostr += alsostr;
        // parse(palsostr);
	// AmyS - 05/16/02. Parse w/o Referred-By in the str.
        parse(alsostr);
    }
    catch (SipReferredByParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in decode() :o( ");
            throw SipReferredByParserException("failed in Decode", __FILE__, __LINE__, DECODE_REFERREDBY_FAILED);

        }



    }
}


SipReferredBy& SipReferredBy::operator = (const SipReferredBy& srcReferredBy)
{
    if ( &srcReferredBy != this )
    {
        referrerUrl = duplicateUrl(srcReferredBy.referrerUrl);
	urlType = srcReferredBy.urlType;
        referencedUrl = srcReferredBy.referencedUrl;
        address = srcReferredBy.address;
        displayName = srcReferredBy.displayName;
    }
    return (*this);
}


bool SipReferredBy::operator ==(const SipReferredBy& src) const
{
    cpDebug(LOG_DEBUG_STACK, "SipReferredBy ::operator ==");

    bool equal = false;

    if ( (referrerUrl.getPtr() != 0) && (src.referrerUrl.getPtr() != 0) )
    {
	equal = ( referrerUrl->areEqual(src.referrerUrl) );
    }
    else if ( (referrerUrl.getPtr() == 0) && (src.referrerUrl.getPtr() == 0) )
    {
	equal = true;
    }
    else
    {
	equal = false;
    }
    
    equal = ( (equal)  &&
	 (referencedUrl == src.referencedUrl) &&
	 (address == src.address) &&
	 (displayName == src.displayName )
	);

    return equal;
}


Data 
SipReferredBy::encode() const
{
    Data sipreferredby;

    sipreferredby = REFERRED_BY;
    sipreferredby += ":";
    sipreferredby += SP;
    if (displayName.length() > 0)
    {
        sipreferredby += displayName;
    }

    if (referrerUrl.getPtr() != 0)
    {
	if (referrerUrl->getType() == SIP_URL)
	{
	    Sptr<SipUrl> sipUrl;
            sipUrl.dynamicCast(referrerUrl);
    
	    if (sipUrl->getHost().length() == 0)
	    {
		sipreferredby += referencedUrl;
		if ( referencedUrl == "sip")
		{
		    sipreferredby += Data(":");
		}
		else
		{
		    sipreferredby += Data("://");
		}
		sipreferredby += address;
		sipreferredby += CRLF;
	    }
	    else
	    {
		sipreferredby += "<";
		sipUrl->encode();
		
		//get before the url param.
		Data nameaddr = sipUrl->getNameAddr();
		
		sipreferredby += nameaddr;
		
		Data userparam = sipUrl->getUrlParam();
		
		if (userparam.length())
		{
		    sipreferredby += userparam;
		}
		
		sipreferredby += ">";
		
//TODO: clean this up
#if 0
		sipreferredby += SP;
		
		sipreferredby += referencedUrl;

		if ( referencedUrl == "sip")
		{
		    sipreferredby += Data(":");
		}
		else
		{
		    sipreferredby += Data("://");
		}
		
		sipreferredby += address;
#endif
		
		if (pgpMap.size() != 0)
		{
		    // support pgp
		    sipreferredby += SP;
		    
		    sipreferredby += authScheme;
		    sipreferredby += SP;
			    
		    
		    tokenMutex.lock();
		    ReferredByPgpMap::const_iterator i = pgpMap.begin();
		    while (i != pgpMap.end())
		    {
			Data token = i->first;
			Data tokenValue = i->second;
			sipreferredby += token;
			sipreferredby += "=";
			sipreferredby += "\"";
			sipreferredby += tokenValue;
			sipreferredby += "\"";
			
			++i;
			if ( i != pgpMap.end())
			{
			    sipreferredby += ",";
			}
		    }  //end while
		    tokenMutex.unlock();
		}
		
		sipreferredby += CRLF;
	    }
	}
    }
    return sipreferredby;
}


Sptr <BaseUrl> 
SipReferredBy::getReferrerUrl() const
{
    return duplicateUrl(referrerUrl, true) ;
}


Data 
SipReferredBy::getReferencedUrl() const
{
    Data data;
    data += address;
    if ( address == "sip")
    {
        data += Data(":");
    }
    else
    {
        data += Data("://");
    }
    data += referencedUrl;
    return data;
}


void 
SipReferredBy::setReferrerUrl(Sptr <BaseUrl> sipurl)
{
    referrerUrl = duplicateUrl(sipurl);
}
    
    
void 
SipReferredBy::setReferencedUrl(const Data& data)
{
    Data sdata = data;
    Data urldata;
    int retn = sdata.match("://", &urldata, true);
    if (retn == FOUND)
    {
        referencedUrl = urldata;
        address = sdata;
    }
    if ( retn == NOT_FOUND)
    {
        Data urdata;
        int ret = sdata.match(":", &urdata, true);
        if ( ret == FOUND)
        {
            referencedUrl = urdata;
            address = sdata;
        }
    }
}



///
void SipReferredBy::setTokenDetails(const Data& token,
                                    const Data& tokenValue)
{
    tokenMutex.lock();
    pgpMap[token] = tokenValue;
    tokenMutex.unlock();
}

///
Sptr < SipReferredBy::ReferredByPgpMap >
SipReferredBy::getTokenDetails()
{
    tokenMutex.lock();
    Sptr < SipReferredBy::ReferredByPgpMap > dupMap = new ReferredByPgpMap(pgpMap);
    tokenMutex.unlock();
    return dupMap;
}


/*
  void SipReferredBy::setBasicCookie(const Data& data)
  {
  basicCookie = data;
  }
  
  
  Data
  SipReferredBy::getBasicCookie()
  {
  return basicCookie;
  }      
*/
Data SipReferredBy::getTokenValue(const Data& token)
{

    Data ret;

    tokenMutex.lock();
    ReferredByPgpMap::iterator check = pgpMap.find(token); ;

    if (check != pgpMap.end())
    {
        ret = check -> second;
    }
    tokenMutex.unlock();
    return ret;

}



void
SipReferredBy::scanAuthTokens( const Data& data)
{
    Data authTokens = data;

    while (authTokens.length())
    {
        Data token, tokenValue;
        int ret = authTokens.match("=", &token, true);
        if (ret == NOT_FOUND)
        {
            if (SipParserMode::sipParserMode())
            {
                throw SipReferredByParserException(
                    "no tokens or challenge",
                    __FILE__,
                    __LINE__,
                    DECODE_REFERREDBY_FAILED);
            }
        }
        else if (ret == FIRST)
        {
            if (SipParserMode::sipParserMode())
            {
                throw SipReferredByParserException(
                    "wrong syntax for token.",
                    __FILE__,
                    __LINE__,
                    DECODE_REFERREDBY_FAILED);
            }
        }
        else
        {
            //found.
            //match the token value.
            int ret = authTokens.match(",", &tokenValue, true);
            if (ret == FOUND)
            {
                //strip "" off this.
                string tokenstr = tokenValue.convertString();
                int pos;
                pos = tokenstr.find("\"");


                if (pos != (int)(string::npos))
                {
                    tokenstr = tokenstr.substr(pos + 1, tokenstr.length() - 2);
                }
                pgpMap[token] = tokenstr;

            }
            else if (ret == NOT_FOUND)
            {
                //add it to map
                //strip "" off this.
                string tokenstr = authTokens.convertString();
                int pos;

                pos = tokenstr.find("\"");

                if (pos != (int)(string::npos))
                {
                    tokenstr = tokenstr.substr(pos + 1, tokenstr.length() - 2);
                }
                Data tmp = tokenstr;
                Data svalue;
                int retur = tmp.match("\"", &svalue, true);
                if (retur == FOUND)
                {
                    pgpMap[token] = svalue;
                }
                else
                {
                    pgpMap[token] = tokenstr;
                }
                break;
            }
            else if (ret == FIRST)
            {
                if (SipParserMode::sipParserMode())
                {
                    throw SipReferredByParserException(
                        "wrong syntax for token.",
                        __FILE__,
                        __LINE__,
                        DECODE_REFERREDBY_FAILED);
                }
            }


        }//else



    }//endwhile
}



SipHeader*
SipReferredBy::duplicate() const
{
    return new SipReferredBy(*this);
}


bool
SipReferredBy::compareSipHeader(SipHeader* msg) const
{
    SipReferredBy* otherMsg = dynamic_cast<SipReferredBy*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}
