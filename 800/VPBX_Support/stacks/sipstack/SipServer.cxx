/*
* $Log: SipServer.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:10  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipServer.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include "SipServer.hxx"
#include "symbols.hxx"
#include "SipParserMode.hxx"

using namespace Assist;


string
SipServerParserException::getName( void ) const
{
    return "SipServerParserException";
}

SipServer::SipServer()
        :
        flgcomment(false),
        flgproduct(false)
{}


SipServer::SipServer( const Data& srcData )
        :
        flgcomment(false),
        flgproduct(false)
{
    data = srcData;
    try
    {
        decode(data);
    }
    catch (SipServerParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            throw SipServerParserException(
                "failed to decode the User Agent string",
                __FILE__,
                __LINE__, DECODE_SERVER_FAILED);
        }
    }
}

void SipServer::decode(const Data& uadata)
{

    Data gdata = uadata;

    try
    {

        scanSipServer(gdata);
    }
    catch (SipServerParserException& exception)
    {
        if (SipParserMode::sipParserMode())
        {
            throw SipServerParserException(
                exception.getDescription(),
                __FILE__,
                __LINE__, SCAN_SERVER_FAILED);

        }
    }

}
void
SipServer::scanSipServer(const Data &tmpdata)
{
    Data agdata = tmpdata;
    Data agvalue;
    int check = agdata.match("(", &agvalue, true);
    if (check == FOUND)
    {
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
                throw SipServerParserException(
                    "failed to decode the User Agent string",
                    __FILE__,
                    __LINE__, DECODE_SERVER_FAILED);
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
                throw SipServerParserException(
                    "failed to decode the User Agent string",
                    __FILE__,
                    __LINE__, DECODE_SERVER_FAILED);
            }
        }
        else if (test == FIRST)
        {
            //
            if (SipParserMode::sipParserMode())
            {
                throw SipServerParserException(
                    "failed to decode the User Agent string",
                    __FILE__,
                    __LINE__, DECODE_SERVER_FAILED);
            }
        }
    }
}
///
Data SipServer::getProduct()
{
    return product;
}


void SipServer::setProduct( const Data& newdata )
{
    product = newdata;
    flgproduct = true;
}

void SipServer::setComment(const Data& newdata)
{
    comment = newdata;
    flgcomment = true;
}
void SipServer::setVersion(const Data& newdata)
{
    version = newdata;

}
Data SipServer::getVersion()
{
    return version;
}
Data SipServer::getComment()
{
    return comment;
}

Data SipServer::encode() const
{
    Data ret;
    if (product.length() || comment.length())
    {
        ret = SIP_SERVER;
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
            ret += "(";
            ret += comment;
            ret += ")";
        }
        ret += CRLF;
    }
    return ret;
}

SipServer::SipServer( const SipServer& src )
{
    data = src.data;
    product = src.product;
    version = src.version;
    comment = src.comment;
    flgcomment = src.flgcomment;
    flgproduct = src.flgproduct;

}

SipServer&
SipServer::operator=( const SipServer& src )
{
    if (&src != this)
    {
        data = src.data;
        product = src.product;
        version = src.version;
        comment = src.comment;
        flgcomment = src.flgcomment;
        flgproduct = src.flgproduct;
    }
    return *this;

}

bool SipServer::operator==( const SipServer& src ) const
{
    
  return 
    (
      (  data == src.data ) &&
      ( product == src.product ) &&
      ( version == src.version ) &&
      ( comment == src.comment ) &&
      ( flgcomment == src.flgcomment ) && 
      ( flgproduct == src.flgproduct ) 
      );
}
///
Data SipServer::get()
{
    return data;
}

///
void SipServer::set( const Data& newdata )
{
    data = newdata;
}



SipHeader*
SipServer::duplicate() const
{
    return new SipServer(*this);
}


bool
SipServer::compareSipHeader(SipHeader* msg) const
{
    SipServer* otherMsg = dynamic_cast<SipServer*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}
