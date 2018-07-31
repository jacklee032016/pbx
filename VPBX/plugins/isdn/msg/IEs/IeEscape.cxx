/*
* $Id: IeEscape.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"
#include "IsdnIeEscape.hxx"
#include "symbols.hxx"
#include "IsdnParserMode.hxx"

using namespace Assist;


string
SipUserAgentParserException::getName( void ) const
{
    return "SipUserAgentParserException";
}
IsdnIeEscape::IsdnIeEscape()
        : flgcomment(false),
        flgproduct(false)
{}


IsdnIeEscape::IsdnIeEscape( const IsdnIeEscape& src )
{
    data = src.data;
    product = src.product;
    version = src.version;
    comment = src.comment;

}


IsdnIeEscape::IsdnIeEscape(const Data& newData)
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
        if (IsdnParserMode::isdnParserMode())
        {
            throw SipUserAgentParserException(
                "failed to decode the User Agent string",
                __FILE__,
                __LINE__, DECODE_USERAGENT_FAILED);
        }
    }
}

void IsdnIeEscape::decode(const Data& uadata)
{
  try
    {

      scanSipUserAgent(uadata);
    }
  catch (SipUserAgentParserException& exception)
    {
      if (IsdnParserMode::isdnParserMode())
        {
	  throw SipUserAgentParserException(
					    exception.getDescription(),
					    __FILE__,
					    __LINE__, SCAN_USERAGENT_FAILED);

        }
    }

}
void
IsdnIeEscape::scanSipUserAgent(const Data &tmpdata)
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
            if (IsdnParserMode::isdnParserMode())
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
            if (IsdnParserMode::isdnParserMode())
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
            if (IsdnParserMode::isdnParserMode())
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
            if (IsdnParserMode::isdnParserMode())
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
const Data& IsdnIeEscape::getProduct() const
{
    return product;
}


void IsdnIeEscape::setProduct( const Data& newdata )
{
    product = newdata;
    flgproduct = true;
}

void IsdnIeEscape::setComment(const Data& newdata)
{
    comment = newdata;
    flgcomment = true;
}
void IsdnIeEscape::setVersion(const Data& newdata)
{
    version = newdata;

}
const Data& IsdnIeEscape::getVersion() const
{
    return version;
}
const Data& IsdnIeEscape::getComment() const
{
    return comment;
}

Data IsdnIeEscape::encode() const
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



const IsdnIeEscape&
IsdnIeEscape::operator=(const IsdnIeEscape& rhs)
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
IsdnIeEscape::operator==(const IsdnIeEscape& rhs) const
{
    return ((data == rhs.data) &&
	    (product == rhs.product) &&
	    (version == rhs.version) &&
	    (comment == rhs.comment) &&
	    (flgcomment == rhs.flgcomment) &&
	    (flgproduct == rhs.flgproduct));
}


IsdnIE*
IsdnIeEscape::duplicate() const
{
    return new IsdnIeEscape(*this);
}


bool
IsdnIeEscape::compareIsdnIE(IsdnIE* msg) const
{
    IsdnIeEscape* otherMsg = dynamic_cast<IsdnIeEscape*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}
/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
