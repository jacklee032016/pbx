/*
* $Log: SipContentType.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipContentType.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"
#include "SipContentType.hxx"
#include "SipParserMode.hxx"
#include "cpLog.h"

using namespace Assist;

string
SipContentTypeParserException::getName( void ) const
{
    return "SipToParserException";
}
    
SipContentType::SipContentType( const SipContentType& src )
    :
    type(src.type),
    subtype(src.subtype),
    myParamList(src.myParamList)
{
   type.lowercase();
   subtype.lowercase();
}


bool
SipContentType::operator ==(const SipContentType& other) const 
{
    if (    (type == other.type) &&
            (subtype == other.subtype) &&
            (myParamList == other.myParamList) 
       )
    {
        return true;
    }
    else
    {
        return false;
    }
}


const SipContentType&
SipContentType::operator=( const SipContentType& src) 
{
    if (&src != this)
    {
        type = src.type;
        subtype = src.subtype;
        myParamList = src.myParamList;
    }

    return *this;
}


SipContentType::SipContentType()
    :
    type(),
    subtype(),
    myParamList(';')
{
    
}

SipContentType::SipContentType(const Data& gtype, const Data& gsubType)
    :
    myParamList(';')
{
    type = gtype;
    subtype = gsubType;
    type.lowercase();
    subtype.lowercase();
}


SipContentType::SipContentType( const Data& data )
    : myParamList(';')
{
    try
    {
        decode(data);
    }
    catch (SipContentTypeParserException&)
    {
	
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");
            throw SipContentTypeParserException("failed in Decode", __FILE__, __LINE__, DECODE_CONTENTTYPE_FAILED);
	    
        }
    }
}


void
SipContentType::scanSipContentType( const Data & adata)
{
    Data sipdata;
    Data data = adata;
    Data appl;
    data.match( ":", &sipdata, true);
  
    int ret = data.match("/", &sipdata, true);
    if (ret == NOT_FOUND)
    {   
	if (SipParserMode::sipParserMode())
	{
	    cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");
	    throw SipContentTypeParserException("failed in Decode", __FILE__, __LINE__, DECODE_CONTENTTYPE_FAILED);
	    
	}	  
    }
    else if (ret == FIRST)
    {
        if (SipParserMode::sipParserMode())
	{
	    cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");
	    throw SipContentTypeParserException("failed in Decode", __FILE__, __LINE__, DECODE_CONTENTTYPE_FAILED);
	    
	}
    }
    else if (ret == FOUND)
    {
	sipdata.removeSpaces();
	setType(sipdata);
    
	Data unparsedData = data;
	Data lsubtype;
	int retn = unparsedData.match(";", &lsubtype, true);
	if (retn == NOT_FOUND)
	{
	    setSubType(unparsedData);
	}
	if (retn == FIRST)
	{
	    if (SipParserMode::sipParserMode())
	    {
		cpLog(LOG_ERR, "Failed to parse subtype :o( ");
		throw SipContentTypeParserException("failed in parse'n subtype ", __FILE__, __LINE__, DECODE_CONTENTTYPE_FAILED);
	    }
	}
	
	if (retn == FOUND)
	{
	    setSubType(lsubtype);
	    Data parms = unparsedData;
            myParamList.decode(parms, ';');
	}
    }
}

    
void
SipContentType::parse( const Data& ctypedata)
{
    Data data = ctypedata;

    try
    {
        Data ndat;
        int remcrlf = data.match ( CRLF, &ndat, true);
        if ( remcrlf == NOT_FOUND)
	{
            scanSipContentType(data);
        }
        else
	{
            scanSipContentType(ndat);
	}
    }
    catch (SipContentTypeParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to parse'n Parms :o( ");
            throw SipContentTypeParserException(
                "failed in parse'n Parms ", 
                __FILE__, __LINE__, 
                DECODE_CONTENTTYPE_FAILED);
        }
    }
    //everything allright.
    type.lowercase();
    subtype.lowercase();
}


void
SipContentType::decode( const Data& ctypestr )
{
    try
    {
        parse(ctypestr);
    }
    catch (SipContentTypeParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to parse'n Parms :o( ");
            throw SipContentTypeParserException(
                "failed in parse'n Parms ", __FILE__, __LINE__, 
                DECODE_CONTENTTYPE_FAILED);
        }
    }
}


Data SipContentType::encode() const
{
    Data content;

    if(type.length() > 0)
    {
	content = CONTENT_TYPE;
	content += SP;
	content += type;
	content += "/";
	if (subtype.length() > 0)
	{
	    content += subtype;
	}

        if(myParamList.size())
        {	
            content+= "; ";
	    content+= myParamList.encode();
	}

	Data scon;
	int rcrlf = content.match(CRLF, &scon, false);
	if ( rcrlf == FIRST)
	{
	    if (SipParserMode::sipParserMode())
	    {
		cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");
		throw SipContentTypeParserException("failed in Decode", 
                                                    __FILE__, __LINE__, 
                                                    DECODE_CONTENTTYPE_FAILED);
		
	    }
	}
	if (rcrlf == NOT_FOUND)
	{
	    content += CRLF;
	}
    }
    return content;
}


void 
SipContentType::setTokenDetails(const Data& token, const Data& tokenValu)
{
    myParamList.setValue(token, tokenValu);
}



void
SipContentType::clearToken(const Data& token)
{
    myParamList.clearValue(token);
}


Data
SipContentType::getType() const 
{ 
    return type; 
}


Data 
SipContentType::getSubType() const 
{ 
    return subtype; 
}


Data SipContentType::getAttributeValue(const Data& token) const
{
    Data ret = myParamList.getValue(token);
    return ret;
}


Data 
SipContentType::getMediatype() 
{ 
    if ( media.length()!=0) 
    {
	return media; 
    }
    else 
    {
	media = type;
	media += "/";
	media += subtype;
	return media;
    }
} 


void 
SipContentType::setMediatype(const Data& data)
{
    Data line1;
    Data cdata = data;
    int value1 = cdata.match("/", &line1, true);
    if (value1 == FOUND)
    {
    type = line1;
    subtype = cdata;
    media = data;
    }
    else
    {
	//throw exception
    }
}


SipHeader*
SipContentType::duplicate() const
{
    return new SipContentType(*this);
}


bool
SipContentType::compareSipHeader(SipHeader* msg) const
{
    SipContentType* otherMsg = dynamic_cast<SipContentType*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}

