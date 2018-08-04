/*
 * $Id: SipMimeVersion.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */

#include "global.h"
#include "symbols.hxx"
#include "SipMimeVersion.hxx"
#include "SipParserMode.hxx"

using namespace Vocal;


string
SipMimeVersionParserException::getName( void ) const
{
    return "SipMimeVersionParserException";
}
SipMimeVersion::SipMimeVersion()
{}



SipMimeVersion::SipMimeVersion( const Data& data ) //TODO
{
    try
    {
        decode(data);
    }
    catch (SipMimeVersionParserException& exception)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode  SipMimeVersion  : ( ");
            throw SipMimeVersionParserException(
                exception.getDescription(),
                __FILE__,
                __LINE__, DECODE_MIMEVERSION_FAILED
            );
        }

    }

}

const SipMimeVersion&
SipMimeVersion::operator=(const SipMimeVersion& src)
{
    if (&src != this)
    {
        value = src.value;
    }
    return (*this);
}

void SipMimeVersion::decode(const Data& data)
{

    Data nData = data;

    try
    {

        scanSipMimeVersion(nData);
    }
    catch (SipMimeVersionParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in decode() of SipMimeVersion  : ( ");
            throw SipMimeVersionParserException(
                exception.getDescription(),
                __FILE__,
                __LINE__, DECODE_MIMEVERSION_FAILED
            );
        }
    }
}

void
SipMimeVersion::scanSipMimeVersion(const Data &tmpdata)
{
    Data newdata = tmpdata ;
    Data rdata;
    int test = newdata.match(".",&rdata,true);
    if (test == FOUND)
      {
	string DIGIT = "0123456789";
	string rsdata = rdata.convertString();
	string newsdata = newdata.convertString();
	int test1 = rsdata.find_first_not_of(DIGIT);
	if (test1 == -1)
	  {
	    int test2 = newsdata.find_first_not_of(DIGIT);
	    if (test2 == -1)
	      {

		set(tmpdata);
	      }
	    else
	      {
		if (SipParserMode::sipParserMode())
		  {
		    cpLog(LOG_ERR, "Failed in parsing DIGITS for MIME-VERSION  : ( ");
		    throw SipMimeVersionParserException(
							"Failed in parsing DIGITS for MIME-VERSION  ",
							__FILE__,
							__LINE__, DECODE_MIMEVERSION_FAILED
							);
		  }
	      }
	  }
	else
	  {
	    if (SipParserMode::sipParserMode())
	      {
		cpLog(LOG_ERR, "Failed in parsing DIGITS for MIME-VERSION  : ( ");
		throw SipMimeVersionParserException(
						    "Failed in parsing  DIGITS for MIME-VERSION  ",
						    __FILE__,
						    __LINE__,DECODE_MIMEVERSION_FAILED
						    );
	      }
	    
	    
	  }
      }
    if (test == NOT_FOUND)
      {
	if (SipParserMode::sipParserMode())
	      {
		cpLog(LOG_ERR, "Failed in parsing DIGITS for MIME-VERSION  : ( ");
		throw SipMimeVersionParserException(
						    "Failed in parsing  DIGITS for MIME-VERSION  ",
						    __FILE__,
						    __LINE__, DECODE_MIMEVERSION_FAILED
						    );
	      }
      }
    if (test == FIRST)
      {
	if (SipParserMode::sipParserMode())
	  {
	    cpLog(LOG_ERR, "Failed in parsing DIGITS for MIME-VERSION  : ( ");
	    throw SipMimeVersionParserException(
						"Failed in parsing  DIGITS for MIME-VERSION  ",
						__FILE__,
						__LINE__, DECODE_MIMEVERSION_FAILED
						);
	  }
	
      }
}
    

SipMimeVersion::SipMimeVersion( const SipMimeVersion& src )
        : value(src.value)
{}

///

Data
SipMimeVersion::get() const
{
    return value;
}

///
void
SipMimeVersion::set( Data newValue )
{
    if (value.length())
    {
        value = newValue;
    }
}

bool SipMimeVersion::operator ==(const SipMimeVersion& srcmv) const
{
    bool equal = false;

    equal = (value == srcmv.value);
    return equal;
}

Data SipMimeVersion::encode() const
{

    Data data;
    if (value.length())
    {
        data = "Mime-Version:";
        data += SP;
        data += value;
        data += CRLF;
    }

    return data;
}



SipHeader*
SipMimeVersion::duplicate() const
{
    return new SipMimeVersion(*this);
}


bool
SipMimeVersion::compareSipHeader(SipHeader* msg) const
{
    SipMimeVersion* otherMsg = dynamic_cast<SipMimeVersion*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}
