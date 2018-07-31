/*
* $Id: IeCentrex.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"
#include "cpLog.h"

#include "IeCentrex.hxx"
#include "IsdnParserMode.hxx"

using namespace Assist;

IeCentrex::IeCentrex()
{}


IeCentrex::IeCentrex( const Data& srcData )
{
    Data fdata = srcData;
    try
    {
        decode(fdata);
    }
    catch (IeCentrexParserException&)
    {

        if (IsdnParserMode::isdnParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor of TimeStamp :( ");
            throw IeCentrexParserException(
                "failed to decode the Timestamp string :(",
                __FILE__,
                __LINE__, DECODE_TIMESTAMP_FAILED);
        }

    }

}
void IeCentrex::decode(const Data& data)
{

    Data nData = data;

    try
    {

        scanIeCentrex(nData);
    }
    catch (IeCentrexParserException&)
    {
        if (IsdnParserMode::isdnParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in TMIESTAMP decode():( ");
            throw IeCentrexParserException(
                "failed to decode the Timestamp string :(",
                __FILE__,
                __LINE__, DECODE_TIMESTAMP_FAILED);
        }
    }

}

IeCentrex::IeCentrex( const IeCentrex& src )
{
    time = src.time;
    delay = src.delay;
}

bool
IeCentrex::operator ==(const IeCentrex& src) const
{
    cpLog(LOG_DEBUG_STACK, "IeCentrex, operator ==");
    if ( (time == src.time) &&
            (delay == src.delay)
       )
    {
        cpLog(LOG_DEBUG_STACK, "IeCentrex,operator == returning true");
        return true;
    }
    else
    {

        cpLog(LOG_DEBUG_STACK, "IeCentrex,operator == returning false");
        return false;
    }
}


int IeCentrex::getTime()
{
    return time.convertInt();
}

void IeCentrex::setTime( const int& newTime )
{
    time = Data(newTime);
}


IsdnIE*
IeCentrex::duplicate() const
{
    return new IeCentrex(*this);
}


bool
IeCentrex::compareIsdnIE(IsdnIE* msg) const
{
    IeCentrex* otherMsg = dynamic_cast<IeCentrex*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}

unsigned char  IeCentrex::encode(msg_t *msg)
{
//void InsIeEncoder::encFacilityCentrex(unsigned char **ntmode, msg_t *msg, unsigned char *cnip, int setup)
	unsigned char centrex[256];
	int i = 0;

	if (!cnip)
		return;

	/* centrex facility */
	centrex[i++] = CENTREX_FAC;
	centrex[i++] = CENTREX_ID;

	/* cnip */
	if (strlen((char *)cnip) > 15)
	{
		PDEBUG(DEBUG_PORT, "%s: CNIP/CONP text too long (max 13 chars), cutting.\n");
		cnip[15] = '\0';
	}
	
	// dunno what the 8 bytes mean
	if (setup)
	{
		centrex[i++] = 0x17;
		centrex[i++] = 0x02;
		centrex[i++] = 0x02;
		centrex[i++] = 0x44;
		centrex[i++] = 0x18;
		centrex[i++] = 0x02;
		centrex[i++] = 0x01;
		centrex[i++] = 0x09;
	}
	else
	{
		centrex[i++] = 0x18;
		centrex[i++] = 0x02;
		centrex[i++] = 0x02;
		centrex[i++] = 0x81;
		centrex[i++] = 0x09;
		centrex[i++] = 0x02;
		centrex[i++] = 0x01;
		centrex[i++] = 0x0a;
	}

	centrex[i++] = 0x80;
	centrex[i++] = strlen((char *)cnip);
	UCPY((char *)(&centrex[i]), (char *)cnip);
	i += strlen((char *)cnip);
	
	printisdn("    cnip='%s'\n", cnip);

	/* encode facility */
	encFacility(ntmode, msg, centrex, i);

}

		
#if WITH_DEBUG
const char *IeCentrex::debugInfo()
{
	int length = 0;
//	const char *tmp;
	char *p = (char *)malloc(DEBUG_BUFFER_SIZE) ;
	assert(p!=0);
	
//	tmp = IeContent::debugInfo();

	length += sprintf( p+ length, "Cause IE info :");
//	length += sprintf( p+ length, "\tContent  : %s", tmp );
//	free((void *)tmp);
	length += sprintf( p+ length, "\tLocation \t : %d", location );
	length += sprintf( p+ length, "\tCause \t : %d", cause );

	return p;
}
#endif		

