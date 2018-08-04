/*
 * $Id: SipMinSE.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */

#include "global.h"
#include "SipMinSE.hxx"
#include "symbols.hxx"
#include "SipParserMode.hxx"
#include "cpLog.h"

using namespace Vocal;


string
SipMinSEParserException::getName( void ) const
{
    return "SipMinSEParserException";
}
SipMinSE::SipMinSE()
        :
        delta(),
        bDelta(false)
{}


SipMinSE::SipMinSE( const Data& srcData )
        :SipHeader(srcData),
        delta(),
        bDelta(false)


{
    Data fdata = srcData;
    try
    {
        decode(fdata);
    }
    catch (SipMinSEParserException&)
    {
        parseError("Failed to decode SipMinSE in Constructor :(");
    }

}
void SipMinSE::decode(const Data& data)
{

    Data nData = data;

    try
    {
        scanSipMinSE(nData);
    }
    catch (SipMinSEParserException exception)
    {
        parseError("Failed to Decode SipMinSE in decode() " );
    }

}

void
SipMinSE::scanSipMinSE(const Data &tmpdata)

{
    Data newdata = tmpdata ;
    Data newvalue;
    //No need to even check for ;, but since it is most common delimiter
    ///check for it anyways
    int ret = newdata.match(";", &newvalue, true);
    if (ret == NOT_FOUND)
    {
        setDelta(newdata);
        bDelta = true;
        return;
    }
    else if ((ret == FIRST) ||(ret == FOUND))
    {
        parseError("Failed to Decode Min-SE in scanSipMinSE");
    }

}




SipMinSE::SipMinSE( const SipMinSE& src )
        :
        delta(src.delta),
        bDelta(src.bDelta)

{}


bool
SipMinSE::operator ==(const SipMinSE& src) const
{
    cpDebug(LOG_DEBUG_STACK, "MinSE == Operator ");
    if ( (delta == src.delta) &&
	 (bDelta == src.bDelta)) 
    {
        cpDebug(LOG_DEBUG_STACK, "MinSE == Operator  returning True :)");
        return true;
    }
    else
    {
        cpDebug(LOG_DEBUG_STACK, "MinSE == Operator  returning false :(");
        return false;
    }
}

const SipMinSE&
SipMinSE::operator=( const SipMinSE& src )
{
    if ( &src != this )
    {
        delta = src.delta;
        bDelta = src.bDelta;
    }
    return *this;
}



void SipMinSE::setDelta ( const Data& data)
{
    delta = data;
    bDelta = true;

    cpDebug(LOG_DEBUG_STACK, "set delta in expires");
}

const Data& SipMinSE::getDelta() const
{
    return delta;
}

///
Data SipMinSE::encode() const
{
    Data data;
    if ( (bDelta) )
    {
        data = MIN_SE;
        data += SP;
        data += delta;
	data += CRLF;
    }
    return data;
}


SipHeader*
SipMinSE::duplicate() const
{
    return new SipMinSE(*this);
}


bool
SipMinSE::compareSipHeader(SipHeader* msg) const
{
    SipMinSE* otherMsg = dynamic_cast<SipMinSE*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}


void
SipMinSE::parseError(const string& context)
{
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "%s", context.c_str());
            throw SipMinSEParserException(context.c_str(),
                __FILE__,
                __LINE__, DECODE_MIN_SE_FAILED);
        }
}
