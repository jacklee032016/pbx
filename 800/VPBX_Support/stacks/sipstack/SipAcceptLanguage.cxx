/*
* $Log: SipAcceptLanguage.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipAcceptLanguage.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"
#include "SipAcceptLanguage.hxx"
#include "SipParserMode.hxx"

using namespace Assist;


string
SipAcceptLanguageParserException::getName( void ) const
{
    return "SipAcceptLanguageParserException";
}
SipAcceptLanguage::SipAcceptLanguage()

{}


SipAcceptLanguage::SipAcceptLanguage( const Data& srcData )

{

    try
    {
        decode(srcData);
    }
    catch (SipAcceptLanguageParserException&)
    {
        if (SipParserMode::sipParserMode())
        {

            throw SipAcceptLanguageParserException(
                "failed to decode theAccept Language string",
                __FILE__,
                __LINE__, DECODE_ACCEPTLANG_FAILED);
        }
    }
}


void SipAcceptLanguage::decode(const Data& vdata)
{
    try
    {

        scanAcceptLanguage(vdata);
    }
    catch (SipAcceptLanguageParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {

            throw SipAcceptLanguageParserException(
                "failed to decode theAccept Language string",
                __FILE__,
                __LINE__, DECODE_ACCEPTLANG_FAILED);
        }
    }

}


void
SipAcceptLanguage::scanAcceptLanguage(const Data & scandata)

{
    Data aedata = scandata;
    Data aevalue;
    int ret = aedata.match(";", &aevalue, true);
    if (ret == FOUND)
    {
        parseRange(aevalue);
        Data qdata = aedata;
        Data qval;
        int retn = qdata.match("=", &qval, true);
        if (retn == FOUND)
        {
            if (qval == QVALUE)
            {
                setqValue(qdata);
            }
            else
            {

                if (SipParserMode::sipParserMode())
                {

                    throw SipAcceptLanguageParserException(
                        "failed to decode theAccept Language string",
                        __FILE__,
                        __LINE__, DECODE_ACCEPTLANG_FAILED);
                }

            }
        }
        else if (retn == NOT_FOUND)
        {

            if (SipParserMode::sipParserMode())
            {

                throw SipAcceptLanguageParserException(
                    "failed to decode theAccept Language string",
                    __FILE__,
                    __LINE__, DECODE_ACCEPTLANG_FAILED);
            }

        }
        else if (retn == FIRST)
        {

            if (SipParserMode::sipParserMode())
            {

                throw SipAcceptLanguageParserException(
                    "failed to decode theAccept Language string",
                    __FILE__,
                    __LINE__, DECODE_ACCEPTLANG_FAILED);
            }

        }
    }

    else if (ret == NOT_FOUND)
    {
        parseRange(aevalue);
    }
    else if (ret == FIRST)
    {
        if (SipParserMode::sipParserMode())
        {

            throw SipAcceptLanguageParserException(
                "failed to decode theAccept Language string",
                __FILE__,
                __LINE__, DECODE_ACCEPTLANG_FAILED);
        }
    }
}


void
SipAcceptLanguage::parseRange( const Data& newdata )
{
    Data rangedata = newdata;
    Data rangeval;
    int ret = rangedata.match("-", &rangeval, true);
    if (ret == FOUND)
    {
        setPrimaryTag(rangeval);
        setSubTag(rangedata);
    }
    else if ( ret == NOT_FOUND)
    {
        setPrimaryTag(rangeval);
    }
    else if (ret == FIRST)
    {

        if (SipParserMode::sipParserMode())
        {

            throw SipAcceptLanguageParserException(
                "failed to decode theAccept Language string",
                __FILE__,
                __LINE__, DECODE_ACCEPTLANG_FAILED);
        }
    }
}


SipAcceptLanguage::SipAcceptLanguage( const SipAcceptLanguage& src)
        : primaryTag(src.primaryTag),
        subTag(src.subTag),
        qValue(src.qValue)
{}


const SipAcceptLanguage&
SipAcceptLanguage::operator =(const SipAcceptLanguage& src)
{
    if (&src != this)
    {
        primaryTag = src.primaryTag;
        subTag = src.subTag;
        qValue = src.qValue;
    }
    return (*this);
}

bool SipAcceptLanguage::operator ==( const SipAcceptLanguage& src) const
{
    return ( (qValue == src.qValue) &&
             (primaryTag == src.primaryTag) &&
             (subTag == src.subTag)
           );
 
}





void SipAcceptLanguage::setPrimaryTag(const Data& srcPrimaryTag)
{
    primaryTag = srcPrimaryTag;
}

Data SipAcceptLanguage::getPrimaryTag() const
{
    return primaryTag;
}

void SipAcceptLanguage::setSubTag(const Data& srcSubTag)
{
    subTag = srcSubTag;
}

Data SipAcceptLanguage::getSubTag() const
{
    return subTag;
}

void SipAcceptLanguage::setqValue(const Data& srcqValue)
{
    qValue = srcqValue;
}

Data SipAcceptLanguage::getqValue() const
{
    return qValue;
}


Data SipAcceptLanguage::encode() const
{
    Data data1;
    if (primaryTag.length())
    {
        data1 = primaryTag;
    }
    if (subTag.length())
    {
        data1 += "-";
        data1 += subTag;
    }
    if (qValue.length())
    {
        data1 += ";";
        data1 += "q=";
        data1 += qValue;
    }
    return data1;

}



SipHeader*
SipAcceptLanguage::duplicate() const
{
    return new SipAcceptLanguage(*this);
}


bool
SipAcceptLanguage::compareSipHeader(SipHeader* msg) const
{
    SipAcceptLanguage* otherMsg = dynamic_cast<SipAcceptLanguage*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}

