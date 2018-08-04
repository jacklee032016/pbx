/*
 * $Id: SipSessionExpires.cxx,v 1.2 2007/03/01 20:04:32 lizhijie Exp $
 */

#include "global.h"
#include "SipSessionExpires.hxx"
#include "symbols.hxx"
#include "SipParserMode.hxx"
#include "cpLog.h"

using namespace Vocal;


string
SipSessionExpiresParserException::getName( void ) const
{
    return "SipSessionExpiresParserException";
}
SipSessionExpires::SipSessionExpires()
        :
        delta(),
        bDelta(false),
        refresher(),
        bRefresher(false)
{}


SipSessionExpires::SipSessionExpires( const Data& srcData )
        :SipHeader(srcData),
        delta(),
        bDelta(false),
        refresher(),
        bRefresher(false)


{
    Data fdata = srcData;
    try
    {
        decode(fdata);
    }
    catch (SipSessionExpiresParserException&)
    {
        parseError("Failed to decode SipSessionExpires in Constructor :(");
    }

}
void SipSessionExpires::decode(const Data& data)
{

    Data nData = data;

    try
    {
        scanSipSessionExpires(nData);
    }
    catch (SipSessionExpiresParserException exception)
    {
        parseError("Failed to Decode SipSessionExpires in decode() " );
    }

}

void
SipSessionExpires::scanSipSessionExpires(const Data &tmpdata)

{
    Data newdata = tmpdata ;
    Data newvalue;
    int ret = newdata.match(";", &newvalue, true);
    if (ret == FOUND)
    {
        setDelta(newvalue);
        bDelta = true;
    }
    else if (ret == NOT_FOUND)
    {
        setDelta(newdata);
        bDelta = true;
        //No parameters
        return;
    }
    else if (ret == FIRST)
    {
        parseError("Failed to Decode Session-Expires in scanSipSessionExpire");
    }

    //Search for parameter
    char matchedChar; 
	// look for an equal sign
	Data key = newdata.matchChar("=", &matchedChar);
	Data value;
	if(matchedChar == '=')
	{
	    value = newdata;
	    newdata.erase();

	    // do something with the key-value pair
            key.removeSpaces();
	    if(key == "refresher")
	    {
		// do something here
		bRefresher = true;
		refresher = value;
                //Value is either "uas" or "uac", other values are invalid
                if ((value != "uas") && (value != "uac"))  
                {
                    parseError("Failed to Decode Session-Expires:");
                }
	    }
	    else 
	    {
                parseError("Unknown parameter in Session-Expires:");
	    }
	}
        else
        {
            parseError("Malformed Session-Expires:");
        }
    if(newdata.length())
    {
        //more data then needed
        parseError("Invalid Session-Expires: header");
    }
}




SipSessionExpires::SipSessionExpires( const SipSessionExpires& src )
        :
        delta(src.delta),
        bDelta(src.bDelta),
        refresher(src.refresher),
        bRefresher(src.bRefresher)

{}


bool
SipSessionExpires::operator ==(const SipSessionExpires& src) const
{
    cpDebug(LOG_DEBUG_STACK, "SessionExpires == Operator ");
    if ( (delta == src.delta) &&
	 (bDelta == src.bDelta) &&
         (refresher == src.refresher) &&
         (bRefresher == src.bRefresher))
    {
        cpDebug(LOG_DEBUG_STACK, "SessionExpireS == Operator  returning True :)");
        return true;
    }
    else
    {
        cpDebug(LOG_DEBUG_STACK, "SessionExpires == Operator  returning false :(");
        return false;
    }
}

const SipSessionExpires&
SipSessionExpires::operator=( const SipSessionExpires& src )
{
    if ( &src != this )
    {
        delta = src.delta;
        bDelta = src.bDelta;
        refresher = src.refresher;
        bRefresher = src.bRefresher;
    }
    return *this;
}



void SipSessionExpires::setDelta ( const Data& data)
{
    delta = data;
    bDelta = true;

    cpDebug(LOG_DEBUG_STACK, "set delta in expires");
}

const Data& SipSessionExpires::getDelta() const
{
    return delta;
}

void SipSessionExpires::setRefreshParam ( const Data& data)
{
    refresher = data;
    bRefresher = true;
}

const Data& SipSessionExpires::getRefreshParam() const
{
    return refresher;
}
///
Data SipSessionExpires::encode() const
{
    Data data;
    if ( (bDelta) )
    {
        data = SESSIONEXPIRES;
        data += SP;
        data += delta;
        if(bRefresher)
        {
            data += SEMICOLON;
            data += SessionTimerParam;
            data += refresher;
        }
	data += CRLF;
    }
    return data;
}


SipHeader*
SipSessionExpires::duplicate() const
{
    return new SipSessionExpires(*this);
}


bool
SipSessionExpires::compareSipHeader(SipHeader* msg) const
{
    SipSessionExpires* otherMsg = dynamic_cast<SipSessionExpires*>(msg);
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
SipSessionExpires::parseError(const string& context)
{
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "%s", context.c_str());
            throw SipSessionExpiresParserException(context.c_str(),
                __FILE__,
                __LINE__, DECODE_SESSION_EXPIRE_FAILED);
        }
}
