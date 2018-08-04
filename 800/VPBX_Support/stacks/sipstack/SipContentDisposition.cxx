/*
* $Log: SipContentDisposition.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipContentDisposition.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"
#include "SipContentDisposition.hxx"
#include "SipParserMode.hxx"
#include "cpLog.h"
#include "symbols.hxx"

static const char SIP_CONTDISPOSITION[] = "Content-Disposition";


using namespace Assist;


string
SipContentDispositionParserException::getName( void ) const
{
    return "SipContentDispositionParserException";
}

SipContentDisposition::SipContentDisposition(const SipContentDisposition & src)
        : attribute(src.attribute),
        value(src.value),
        handleparm(src.handleparm),
	dispositiontype(src.dispositiontype)
{
}


SipContentDisposition::SipContentDisposition(void)

{
}


SipContentDisposition::SipContentDisposition( const Data& newData )

{
    try
    {
        decode(newData);
    }
    catch (SipContentDispositionParserException&)
    {

        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");
            throw SipContentDispositionParserException("failed in Decode", __FILE__, __LINE__, DECODE_CONTENTDISPOSITION_FAILED);

        }

    }

}
void SipContentDisposition::decode(const Data& data)
{

    Data nData = data;

    try
    {

        scanSipContentDisposition(nData);
    }
    catch (SipContentDispositionParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");
            throw SipContentDispositionParserException("failed in Decode", __FILE__, __LINE__, DECODE_CONTENTDISPOSITION_FAILED);

        }

    }

}

void
SipContentDisposition::scanSipContentDisposition(const Data &tmpdata)
{
    Data despdata = tmpdata;
    Data despvalue;
    int test = despdata.match(";", &despvalue, true);
    if (test == FOUND)
    {
        setDispositionType(despvalue);
        parseDispositionParm(despdata);
    }
    else if (test == NOT_FOUND)
    {
        setDispositionType(despdata);
    }
    else if (test == FIRST)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");
            throw SipContentDispositionParserException("failed in Decode", __FILE__, __LINE__, DECODE_CONTENTDISPOSITION_FAILED);

        }
    }

}

void
SipContentDisposition::parseDispositionParm(const Data &tmepdata)
{

    Data parmdata = tmepdata;
    Data parmvalue;
    while (parmdata.length())
    {
        int test = parmdata.match(";", &parmvalue, true);
        if (test == FOUND)
        {
            parseParms(parmvalue);

        }
        else if (test == NOT_FOUND)
        {
            parseParms(parmdata);
            break;
        }
        else if (test == FIRST)
        {}

    }

}
void
SipContentDisposition::parseParms(const Data &tpdata)
{
    Data dsdata = tpdata;
    Data dsvalue;
    int test = dsdata.match("=", &dsvalue, true);
    if (test == FOUND)
    {
        parseFinParms(dsvalue, dsdata);
    }
    else if (test == NOT_FOUND)
    {

    }

    else if (test == FIRST)
    {
    }

}

SipContentDisposition& SipContentDisposition::operator= ( const SipContentDisposition & src)
{
    if ( &src != this )
    {
        attribute = src.attribute;
        value = src.value;
        handleparm = src.handleparm;
        dispositiontype = src.dispositiontype;
    }
    return (*this);
}

bool SipContentDisposition::operator== ( const SipContentDisposition & src) const
{
    return (
       ( attribute == src.attribute ) && 
       ( value == src.value ) && 
       ( handleparm == src.handleparm ) &&
       ( dispositiontype == src.dispositiontype )    );
}


void
SipContentDisposition::parseFinParms(const Data &tpdata, const Data &tvalue)
{
    Data dspdata = tpdata;
    Data dspvalue = tvalue;
    if (dspdata == HANDLING_PARM)
    {
        if (dspvalue == CONT_OPTIONAL || dspvalue == CONT_REQUIRED)
        {
            setHandleParm(dspvalue);
        }
        else
        {
            if (SipParserMode::sipParserMode())
            {
                cpLog(LOG_ERR, "Failed to Decode in parseFinParms Handle Parm is not (optional | required) :o( ");
                throw SipContentDispositionParserException("failed in Decode", __FILE__, __LINE__, DECODE_CONTENTDISPOSITION_FAILED);

            }

        }
    }
    else

    {
        setAttribute(dspdata);
        setValue(dspvalue);
    }
}



Data SipContentDisposition::encode() const
{
    Data ret;
    Data dispType = getDispositionType();
    Data handleParm = getHandleParm();
    Data attr = getAttribute();
    Data val = getValue();

    if ( (dispType.length()) ||
	 (handleParm.length()) ||
	 (attr.length())
       )
    {
        ret += SIP_CONTDISPOSITION ;
        ret += ": ";
        ret += dispType;
        if (handleParm.length())
	{
            ret += ";";
            ret += HANDLING_PARM;
            ret += "=";
            ret += handleParm;
	}
        if ( (attr.length()) &&
             (val.length())
            )
        {
            ret += ";";
            ret += attr;
            ret += "=";
            ret += val;
        }
        ret += CRLF;
    }
    return ret;

}


SipHeader*
SipContentDisposition::duplicate() const
{
    return new SipContentDisposition(*this);
}


bool
SipContentDisposition::compareSipHeader(SipHeader* msg) const
{
    SipContentDisposition* otherMsg = dynamic_cast<SipContentDisposition*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}

