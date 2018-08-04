/*
* $Log: SipContentLanguage.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipContentLanguage.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"
#include "SipContentLanguage.hxx"
#include "symbols.hxx"
#include "SipParserMode.hxx"
#include "cpLog.h"

using namespace Assist;


string
SipContentLanguageParserException::getName( void ) const
{
    return "SipContentLanguageParserException";
}

SipContentLanguage::SipContentLanguage()
{}


SipContentLanguage::SipContentLanguage( const SipContentLanguage & src)
{
    primarytag = src.primarytag;
    subtag = src.subtag;
    
}

SipContentLanguage& SipContentLanguage::operator=(const SipContentLanguage& src)
{
    if ( &src != this)
    {
        primarytag = src.primarytag;
	subtag = src.subtag;
    }
    return (*this);
}

SipContentLanguage::SipContentLanguage( const Data& srcData )
  :primarytag(),
   subtag()
{
    Data wdata = srcData;
    try
    {
        decode(wdata);
    }
    catch (SipContentLanguageParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor of ContentLanguage :( ");
            throw SipContentLanguageParserException(
                "failed to decode the Subject string :(",
                __FILE__,
                __LINE__, DECODE_CONTENTLANGUAGE_FAILED);
        }


    }

}
void SipContentLanguage::decode(const Data& data)
{

    Data nData = data;

    try
    {

        scanSipContentLanguage(nData);
    }
    catch (SipContentLanguageParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in decode method of ContentLanguage  :( ");
            throw SipContentLanguageParserException(
                exception.getDescription(),
                __FILE__,
                __LINE__, DECODE_CONTENTLANGUAGE_FAILED
            );
        }
    }
}

void
SipContentLanguage::scanSipContentLanguage(const Data &tmpdata)
{
   Data workdat = tmpdata;
   Data nextdat;
   int test = workdat.match("-", &nextdat,true);
   if (test == FOUND)
     {
       setPrimaryTag(nextdat);
       setSubTag(workdat);
     }
   else if (test == NOT_FOUND)
     {
       setPrimaryTag(workdat);
     }
   else if (test == FIRST)
     {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in method scanSipContentLanguage :( ");
            throw SipContentLanguageParserException("Failed to Decode in method scanSipContentLanguage", __FILE__,__LINE__, DECODE_CONTENTLANGUAGE_FAILED);
        }
     }
}



Data SipContentLanguage::getPrimaryTag()
{
    return primarytag;
}
Data SipContentLanguage::getSubTag()
{
    return subtag;
}

void SipContentLanguage::setPrimaryTag( const Data& newdata)
{
    primarytag = newdata;
}
void SipContentLanguage::setSubTag( const Data& nwdata)
{
    subtag = nwdata;
}

Data SipContentLanguage::encode() const
{
    Data ret;
    if (primarytag.length())
    {
        ret = CONTENTLANGUAGE;
        ret += SP;
	ret += primarytag;
	if (subtag.length())
	  {
	     ret += "-";
	     ret += subtag;
	  }
        ret += CRLF;
    }
    return ret;
}

bool SipContentLanguage::operator==(const SipContentLanguage& src) const
{
    if ( (primarytag == src.primarytag) &&
         (subtag == src.subtag)
       )
      {
	  return true;
      }
      else
      {
	  return false;
      }
}



SipHeader*
SipContentLanguage::duplicate() const
{
    return new SipContentLanguage(*this);
}


bool
SipContentLanguage::compareSipHeader(SipHeader* msg) const
{
    SipContentLanguage* otherMsg = dynamic_cast<SipContentLanguage*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
} 
