/*
 * $Id: SipAcceptEncoding.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */

#include "global.h"
#include "SipAcceptEncoding.hxx"
#include "SipParserMode.hxx"

using namespace Vocal;


string
SipAcceptEncodingParserException::getName( void ) const
{
    return "SipAcceptEncodingParserException";
}


///
SipAcceptEncoding::SipAcceptEncoding()
{}

///

SipAcceptEncoding::SipAcceptEncoding( const Data& srcData )
{
    try
    {
        decode(srcData);
    }
    catch (SipAcceptEncodingParserException&)
    {
        if (SipParserMode::sipParserMode())
        {

            throw SipAcceptEncodingParserException(
                "failed to decode the AcceptEncoding string :(",
                __FILE__,
                __LINE__, DECODE_ACCEPTENCODING_FAILED);
        }

    }


}

void SipAcceptEncoding::decode(const Data& vdata)
{

    try
    {

        scanAcceptEncoding(vdata);
    }
    catch (SipAcceptEncodingParserException exception)
    {

        if (SipParserMode::sipParserMode())
        {

            throw SipAcceptEncodingParserException(
                "failed to decode the AcceptEncoding string :(",
                __FILE__,
                __LINE__, DECODE_ACCEPTENCODING_FAILED);
        }


    }

}

void
SipAcceptEncoding::scanAcceptEncoding(const Data & scandata)
{
    Data aedata = scandata;
    Data aevalue;
    int ret = aedata.match(SEMICOLON, &aevalue, true);
    if (ret == FOUND)
    {
        
        Data qdata = aedata;
        Data qval;
        int retn = qdata.match(EQUAL, &qval, true);
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

                    throw SipAcceptEncodingParserException(
                        "failed to decode the AcceptEncoding string :(",
                        __FILE__,
                        __LINE__, DECODE_ACCEPTENCODING_FAILED);
                }

            }
        }
        else if (retn == NOT_FOUND)
        {
            if (SipParserMode::sipParserMode())
            {

                throw SipAcceptEncodingParserException(
                    "failed to decode the AcceptEncoding string :(",
                    __FILE__,
                    __LINE__, DECODE_ACCEPTENCODING_FAILED);
            }

        }
        else if (retn == FIRST)
        {
            if (SipParserMode::sipParserMode())
            {

                throw SipAcceptEncodingParserException(
                    "failed to decode the AcceptEncoding string :(",
                    __FILE__,
                    __LINE__, DECODE_ACCEPTENCODING_FAILED);
            }

        }
    }

    else if (ret == NOT_FOUND)
    {
      //do nothing
    }
    else if (ret == FIRST)
    {

        if (SipParserMode::sipParserMode())
        {

            throw SipAcceptEncodingParserException(
                "failed to decode the AcceptEncoding string :(",
                __FILE__,
                __LINE__, DECODE_ACCEPTENCODING_FAILED);
        }

    }

}
///
SipAcceptEncoding::SipAcceptEncoding( const SipAcceptEncoding& src)
        : 
        contentCoding(src.contentCoding),
        qValue(src.qValue)
{}


const SipAcceptEncoding&
SipAcceptEncoding::operator =(const SipAcceptEncoding& src)
{
    if (&src != this)
    {
        contentCoding = src.contentCoding;
        qValue = src.qValue;
    }
    return (*this);
}

bool SipAcceptEncoding::operator ==( const SipAcceptEncoding& src) const
{
    return ( (contentCoding == src.contentCoding) &&
             (qValue == src.qValue)
           );
}


Data SipAcceptEncoding::getContentCoding() const
{
    return contentCoding;
}

void SipAcceptEncoding::setContentCoding(const Data& srcContentCoding)
{
    contentCoding = srcContentCoding;
}

Data SipAcceptEncoding::getqValue() const
{
    return qValue;
}

void SipAcceptEncoding::setqValue(const Data& srcqValue)
{
    qValue = srcqValue;
}



///
Data SipAcceptEncoding::encode() const
{
    Data data2;
    if (contentCoding.length())
    {
        data2 = contentCoding;
        if (qValue.length())
        {
            data2 += SEMICOLON;
            data2 += "q=";
            data2 += qValue;
        }
    }
    return data2;

}




SipHeader*
SipAcceptEncoding::duplicate() const
{
    return new SipAcceptEncoding(*this);
}


bool
SipAcceptEncoding::compareSipHeader(SipHeader* msg) const
{
    SipAcceptEncoding* otherMsg = dynamic_cast<SipAcceptEncoding*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}
