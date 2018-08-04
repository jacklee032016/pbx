/*
* $Log: SipAccept.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipAccept.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"
#include "symbols.hxx"
#include "SipAccept.hxx"
#include "Data.hxx"
#include "SipParserMode.hxx"

using namespace Assist;

string
SipAcceptParserException::getName( void ) const
{
    return "SipAcceptParserException";
}

SipAccept::SipAccept()
        :
        allmedia(false),
        flagmedia(false)
{}

SipAccept::SipAccept(const Data& newaccept)
        :
        allmedia(false),
        flagmedia(false)
{
    try
    {
        decode(newaccept);
    }
    catch (SipAcceptParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            throw SipAcceptParserException(
                "failed to decode the Priority string",
                __FILE__,
                __LINE__, DECODE_ACCEPT_FAILED);
        }
    }
}


void SipAccept::decode(const Data& acceptdata)
{
    Data data = acceptdata;

    try
    {
        scanAccept(data);
    }
    catch (SipAcceptParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            throw SipAcceptParserException(
                "failed to decode the Priority string",
                __FILE__,
                __LINE__, DECODE_ACCEPT_FAILED);
        }
    }
}


void
SipAccept::scanAccept(const Data &tmpdata)
{
    Data accdata;
    Data aptdata = tmpdata;
    int ret = aptdata.match("/", &accdata, true);
    if (ret == FOUND)
    {
        if (accdata == ASTERISK)
        {
            Data acptdata = aptdata;
            Data tkdata;
            int retr = acptdata.match(";", &tkdata, true);
            if (retr == FOUND)
            {
                if (tkdata == ASTERISK)
                {
                    allmedia = true;
                    parseMediaParm(acptdata);
                }
                else
                {
                    if (SipParserMode::sipParserMode())
                    {
                        throw SipAcceptParserException(
                            "failed to decode the Priority string",
                            __FILE__,
                            __LINE__, DECODE_ACCEPT_FAILED);
                    }  //exception
                }

            }
            else if (retr == NOT_FOUND)
            {
                // Since it's optional
                if (acptdata == ASTERISK)
                {
                    allmedia = true;
                }
                else
                {
                    if (SipParserMode::sipParserMode())
                    {

                        throw SipAcceptParserException(
                            "failed to decode the Accept string",
                            __FILE__,
                            __LINE__, DECODE_ACCEPT_FAILED);
                    }  //exceptions because "*/*" is not satisfied
                }

            }
            else if (retr == FIRST)
            {
                if (SipParserMode::sipParserMode())
                {
                    throw SipAcceptParserException(
                        "failed to decode the Accept string",
                        __FILE__,
                        __LINE__, DECODE_ACCEPT_FAILED);
                }
            }
        }
        else
        {
            setMediaType(accdata);
            Data fdata = aptdata;
            Data kdata ;
            int re = fdata.match(";", &kdata, true);
            if (re == FOUND)
            {
                if (kdata == ASTERISK)
                {
                    flagmedia = true;
                    parseMediaParm(fdata);
                }
                else
                {
                    setMediaSubtype(kdata);

                    string testdata = fdata.convertString();
                    int test1 = testdata.find("q");
                    if (test1 == 0)
                    {
                        parseAcceptParm(fdata);
                    }
                    else
                    {
                        parseMediaParm(fdata);
                    }
                }
            }
            else if (re == NOT_FOUND)
            {
                // Since it's optional
                if (kdata == ASTERISK)
                {
                    flagmedia = true;
                }
                else
                {
                    setMediaSubtype(fdata);
                }
            }
            else if (re == FIRST)
            {
                if (SipParserMode::sipParserMode())
                {
                    throw SipAcceptParserException(
                        "failed to decode the Accept string",
                        __FILE__,
                        __LINE__, DECODE_ACCEPT_FAILED);
                }   //excpetion
            }

        }
    }
    else if (ret == NOT_FOUND)
    {
        if (SipParserMode::sipParserMode())
        {
            throw SipAcceptParserException(
                "failed to decode the Accept string",
                __FILE__,
                __LINE__, DECODE_ACCEPT_FAILED);
        }  //
    }
    else if (ret == FIRST)
    {
        if (SipParserMode::sipParserMode())
        {
            throw SipAcceptParserException(
                "failed to decode the Accept string",
                __FILE__,
                __LINE__, DECODE_ACCEPT_FAILED);
        }
        // //exceptions
    }
}



void
SipAccept::parseMediaParm(const Data& tmpdata)
{
    Data apctdata = tmpdata;
    Data findata;
    string testdata = tmpdata.convertString();
    int test1 = testdata.find("q");
    if (test1 == 0)
    {
        parseAcceptParm(apctdata);
    }
    else
    {
        while (apctdata.length())
        {
            string testdata = apctdata.convertString();
            int test2 = testdata.find("q");
            if (test2 == 0)
            {
                parseAcceptParm(apctdata);
                break;
            }

            int ret = apctdata.match(SEMICOLON, &findata, true);
            if (ret == FOUND)
            {
                parseMediaParms(findata, apctdata);
            }
            else if (ret == FIRST)
            {
                if (SipParserMode::sipParserMode())
                {

                    throw SipAcceptParserException(
                        "failed to decode the Accept string",
                        __FILE__,
                        __LINE__, DECODE_ACCEPT_FAILED);
                }
            }
            else if (ret == NOT_FOUND)
            {
                Data pvalue;
                Data pdata = apctdata;
                int check = pdata.match("=", &pvalue , true);

                if ( check == NOT_FOUND)
                {
                    //Exceptions
                }

                else if (check == FIRST)
                {

                    if (SipParserMode::sipParserMode())
                    {

                        throw SipAcceptParserException(
                            "failed to decode the Accept string",
                            __FILE__,
                            __LINE__, DECODE_ACCEPT_FAILED);
                    }
                }
                else if (check == FOUND)

                    //value has the correct

                    if (pdata.length())
                    {
                        try
                        {
                            parseMediaExtension(pvalue, pdata);
                        }
                        catch (SipAcceptParserException&)
                        {
                            if (SipParserMode::sipParserMode())
                            {

                                throw SipAcceptParserException(
                                    "failed to decode the Accept string",
                                    __FILE__,
                                    __LINE__, DECODE_ACCEPT_FAILED);
                            }


                        }
                    }
                break;

            }

        }
    }
}

void
SipAccept::parseMediaParms(const Data& data, const Data& value)
{

    Data pvalue = value;
    Data pdata = data;
    string testdata = data.convertString();
    int test1 = testdata.find("q");
    if (test1 == 0)
    {
        parseAcceptParm(pdata);
    }

    // Check for the Parms With Constant "=" sign
    int check = pdata.match("=", &pvalue , true);

    if ( check == NOT_FOUND)
    {
        //Exceptions
    }

    else if (check == FIRST)
    {

        if (SipParserMode::sipParserMode())
        {

            throw SipAcceptParserException(
                "failed to decode the Accept string",
                __FILE__,
                __LINE__, DECODE_ACCEPT_FAILED);
        }

    }
    else if (check == FOUND)

        //value has the correct

        if (pdata.length())
        {
            try
            {
                parseMediaExtension(pvalue, pdata);
            }
            catch (SipAcceptParserException&)
            {

                if (SipParserMode::sipParserMode())
                {

                    throw SipAcceptParserException(
                        "failed to decode the Accept string",
                        __FILE__,
                        __LINE__, DECODE_ACCEPT_FAILED);
                }

            }
        }
}


void
SipAccept::parseMediaExtension(const Data& extensiondata, const Data& extensionvalue)
{
    Data mdiatokenparm = extensiondata ;
    Data mdiatokenvalue = extensionvalue;
    setMediaToken(mdiatokenparm);
    setMediaTokenValue(mdiatokenvalue);


}

void
SipAccept::parseAcceptParm(const Data & tempdata)
{
    Data tempvalue;
    Data acparm = tempdata;
    while (acparm.length())
    {
        int ret = acparm.match(SEMICOLON, &tempvalue, true);
        if (ret == FOUND)
        {
            parseAcceptParms(tempvalue);
        }
        else if (ret == FIRST)
        {
            if (SipParserMode::sipParserMode())
            {

                throw SipAcceptParserException(
                    "failed to decode the Accept string",
                    __FILE__,
                    __LINE__, DECODE_ACCEPT_FAILED);
            }
        }
        else if (ret == NOT_FOUND)
        {
            Data pvalue;
            Data pdata = acparm;
            int check = pdata.match("=", &pvalue , true);

            if ( check == NOT_FOUND)
            {
                setMediaToken(pvalue);
            }
            else if (check == FIRST)
            {
                if (SipParserMode::sipParserMode())
                {

                    throw SipAcceptParserException(
                        "failed to decode the Accept string",
                        __FILE__,
                        __LINE__, DECODE_ACCEPT_FAILED);
                }


            }
            else if (check == FOUND)

                //value has the correct

                if (pdata.length())
                {
                    try
                    {
                        parseAcceptExtension(pvalue, pdata);
                    }
                    catch (SipAcceptParserException&)
                    {
                        if (SipParserMode::sipParserMode())
                        {

                            throw SipAcceptParserException(
                                "failed to decode the Accept  string",
                                __FILE__,
                                __LINE__, DECODE_ACCEPT_FAILED);
                        }


                    }
                }
            break;

        }

    }
}
void
SipAccept::parseAcceptParms(const Data& data)
{

    Data pvalue;
    Data pdata = data;
    // Check for the Parms With Constant "=" sign
    int check = pdata.match("=", &pvalue , true);

    if ( check == NOT_FOUND)
    {
        setAcceptExtensionParm(pvalue);
    }
    else if (check == FOUND)
    {
        parseAcceptExtension(pvalue, pdata);
    }
    else //if (check == FIRST)
    {
        //do nothing
    }
}



void
SipAccept::parseAcceptExtension(const Data& extensiondata, const Data& extensionvalue)
{
    if (extensiondata == QVALUE)
    {
        qValue = extensionvalue;
    }
    else
    {
        setAcceptExtensionParm(extensiondata);
        setAcceptExtensionValue(extensionvalue);
    }

}

const SipAccept& SipAccept::operator =(const SipAccept& src)
{
    if (&src != this)
    {
        mediaType = src.mediaType;
        mediaSubtype = src.mediaSubtype;
        qValue = src.qValue;
        mediatoken = src.mediatoken;
        mediatokenValue = src.mediatokenValue;
        accepttoken = src.accepttoken;
        accepttokenValue = src.accepttokenValue;
        allmedia = src.allmedia;
        flagmedia = src.flagmedia;
    }
    return (*this);
}

bool SipAccept::operator == (const SipAccept& src) const
{
    if (
        (mediaType == src.mediaType) &&
        (mediaSubtype == src.mediaSubtype) &&
        (qValue == src.qValue) &&
        (mediatoken == src.mediatoken) &&
        (mediatokenValue == src.mediatokenValue) &&
        (accepttoken == src.accepttoken) &&
        (accepttokenValue == src.accepttokenValue) &&
        (allmedia == src.allmedia) &&
        (flagmedia == src.flagmedia)
    )
    {
        return true;
    }
    else
    {
        return false;
    }
}

SipAccept::SipAccept( const SipAccept& src )
        : mediaType(src.mediaType),
        mediaSubtype(src.mediaSubtype),
        qValue(src.qValue),
        mediatoken(src.mediatoken),
        mediatokenValue(src.mediatokenValue),
        accepttoken(src.accepttoken),
        accepttokenValue(src.accepttokenValue),
        allmedia(src.allmedia),
        flagmedia(src.flagmedia)
{}


Data 
SipAccept::getMediaType() const
{
    return mediaType;
}

void 
SipAccept::setMediaType(const Data& srcmediaType)
{
    mediaType = srcmediaType;
    //when mediaType is set, the subTYpe is set as *.
    mediaSubtype = ALL_MEDIASUBTYPES;
}

Data 
SipAccept::getMediaSubtype() const
{
    return mediaSubtype;
}

void 
SipAccept::setMediaSubtype(const Data& srcmediaSubtype)
{
    mediaSubtype = srcmediaSubtype;
}

Data 
SipAccept::getqValue() const
{
    return qValue;
}

void 
SipAccept::setqValue(const Data& srcqValue)
{
    qValue = srcqValue;
}

Data 
SipAccept::getMediaToken() const
{
    return mediatoken;
}

void 
SipAccept::setMediaToken(const Data& srcToken)
{
    mediatoken = srcToken;
}

Data 
SipAccept::getMediaTokenValue() const
{
    return mediatokenValue;
}

void 
SipAccept::setMediaTokenValue(const Data& srcTokenValue)
{
    mediatokenValue = srcTokenValue;
}


void 
SipAccept::setAcceptExtensionParm(const Data& newaccepttoken)
{
    accepttoken = newaccepttoken;
}


void 
SipAccept::setAcceptExtensionValue(const Data& newaccepttokenvalue)
{
    accepttokenValue = newaccepttokenvalue;
}


Data 
SipAccept::getAccpetExtensionParm() const
{
    return accepttoken;
}


Data 
SipAccept::getAccpetExtensionValue() const
{
    return accepttokenValue;
}


/*** return the encoded string version of this. This call should only be
     used inside the stack and is not part of the API */
Data 
SipAccept::encode() const
{
    Data data;
    if (allmedia)
    {
        data = "*";
        data += "/";
        data += "*";
    }
    if (mediaType.length())
    {
        data = mediaType;
        data += "/";
        data += mediaSubtype;
    }

    if ( (mediatoken.length()) && (mediatokenValue.length()) )
    {
        data += SEMICOLON;
        data += mediatoken;
        data += "=";
        data += mediatokenValue;
    }

    if (qValue.length())
    {
        data += SEMICOLON;
        data += "q=";
        data += qValue;
    }

    if ((accepttoken.length()) && (accepttokenValue.length() == 0) )
    {
        data += SEMICOLON;
        data += accepttoken;

    }
    if ((accepttoken.length()) && (accepttokenValue.length()))
    {
        data += SEMICOLON;
        data += accepttoken;
        data += "=";
        data += accepttokenValue;
    }
    return data;
}






SipHeader*
SipAccept::duplicate() const
{
    return new SipAccept(*this);
}


bool
SipAccept::compareSipHeader(SipHeader* msg) const
{
    SipAccept* otherMsg = dynamic_cast<SipAccept*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}

