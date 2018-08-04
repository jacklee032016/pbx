/*
* $Log: SipUserAgent.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:10  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipUserAgent.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/
#include "global.h"
#include "SipUserAgent.hxx"
#include "symbols.hxx"
#include "SipParserMode.hxx"

using namespace Assist;


string
SipUserAgentParserException::getName( void ) const
{
    return "SipUserAgentParserException";
}
SipUserAgent::SipUserAgent()
        : flgcomment(false),
        flgproduct(false)
{}


SipUserAgent::SipUserAgent( const SipUserAgent& src )
{
    data = src.data;
    product = src.product;
    version = src.version;
    comment = src.comment;

}


SipUserAgent::SipUserAgent(const Data& newData)
        : flgcomment(false),
        flgproduct(false)

{

    data = newData;
    try
    {
        decode(data);
    }
    catch (SipUserAgentParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            throw SipUserAgentParserException(
                "failed to decode the User Agent string",
                __FILE__,
                __LINE__, DECODE_USERAGENT_FAILED);
        }
    }
}
void SipUserAgent::decode(const Data& uadata)
{
  try
    {

      scanSipUserAgent(uadata);
    }
  catch (SipUserAgentParserException& exception)
    {
      if (SipParserMode::sipParserMode())
        {
	  throw SipUserAgentParserException(
					    exception.getDescription(),
					    __FILE__,
					    __LINE__, SCAN_USERAGENT_FAILED);

        }
    }

}
void
SipUserAgent::scanSipUserAgent(const Data &tmpdata)
{
    Data agdata = tmpdata;
    Data agvalue;
    int check = agdata.match("(", &agvalue, true);
    if (check == FOUND)
    {
      Data j1data = agvalue;
      Data j1value;
      int ret1 = j1data.match("/", &j1value, true);
      if (ret1 == FOUND)
        {
	  setProduct(j1value);
	  setVersion(j1data);
        }
      else if (ret1 == NOT_FOUND)
        {
	  setProduct(j1data);
        }
        else if (ret1 == FIRST)
        {
            if (SipParserMode::sipParserMode())
            {
                throw SipUserAgentParserException(
                    "failed to decode the User Agent string",
                    __FILE__,
                    __LINE__, DECODE_USERAGENT_FAILED);
            }
        }
     
        Data avalue;
	int test = agdata.match(")", &avalue,true);
	if (test == FOUND)
	{
	  setComment(avalue);
 	}
	if (test == NOT_FOUND)
	{
	
	}
	if (test == FIRST)
	{
	
	}
    }

    else if (check == NOT_FOUND)
    {
        Data jdata = agdata;
        Data jvalue;
        int ret = jdata.match("/", &jvalue, true);
        if (ret == FOUND)
        {
            setProduct(jvalue);
            setVersion(jdata);
        }
        else if (ret == NOT_FOUND)
        {
            setProduct(jdata);
        }
        else if (ret == FIRST)
        {
            if (SipParserMode::sipParserMode())
            {
                throw SipUserAgentParserException(
                    "failed to decode the User Agent string",
                    __FILE__,
                    __LINE__, DECODE_USERAGENT_FAILED);
            }
        }
    }
    else if (check == FIRST)
    {
        Data ndata = agdata;
        Data nvalue;
        int test = ndata.match(")", &nvalue, true);
        if (test == FOUND)
        {
            setComment(nvalue);
        }
        else if (test == NOT_FOUND)
        {
            //
            if (SipParserMode::sipParserMode())
            {
                throw SipUserAgentParserException(
                    "failed to decode the User Agent string",
                    __FILE__,
                    __LINE__, DECODE_USERAGENT_FAILED);
            }
        }
        else if (test == FIRST)
        {
            //
            if (SipParserMode::sipParserMode())
            {
                throw SipUserAgentParserException(
                    "failed to decode the User Agent string",
                    __FILE__,
                    __LINE__, DECODE_USERAGENT_FAILED);
            }
        }
    }
}
///
const Data& SipUserAgent::getProduct() const
{
    return product;
}


void SipUserAgent::setProduct( const Data& newdata )
{
    product = newdata;
    flgproduct = true;
}

void SipUserAgent::setComment(const Data& newdata)
{
    comment = newdata;
    flgcomment = true;
}
void SipUserAgent::setVersion(const Data& newdata)
{
    version = newdata;

}
const Data& SipUserAgent::getVersion() const
{
    return version;
}
const Data& SipUserAgent::getComment() const
{
    return comment;
}

Data SipUserAgent::encode() const
{
    Data ret;
    if (product.length() || comment.length())
    {
        ret = USERAGENT;
        ret += SP;
        if (product.length())
        {
            ret += product;
        }
        if (version.length())
        {
            ret += "/";
            ret += version;
        }
        if (comment.length())
        {
            ret += " ";
            ret += "(";
            ret += comment;
            ret += ")";
        }
        ret += CRLF;
    }
    return ret;
}



const SipUserAgent&
SipUserAgent::operator=(const SipUserAgent& rhs)
{
    if (&rhs != this)
    {
        data = rhs.data;
        product = rhs.product;
        version = rhs.version;
        comment = rhs.comment;
        flgcomment = rhs.flgcomment;
        flgproduct = rhs.flgproduct;
    }
    return *this;
}


bool
SipUserAgent::operator==(const SipUserAgent& rhs) const
{
    return ((data == rhs.data) &&
	    (product == rhs.product) &&
	    (version == rhs.version) &&
	    (comment == rhs.comment) &&
	    (flgcomment == rhs.flgcomment) &&
	    (flgproduct == rhs.flgproduct));
}


SipHeader*
SipUserAgent::duplicate() const
{
    return new SipUserAgent(*this);
}


bool
SipUserAgent::compareSipHeader(SipHeader* msg) const
{
    SipUserAgent* otherMsg = dynamic_cast<SipUserAgent*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}
