/*
 * $Id: Sdp2Time.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */


#include "global.h"
#include <cassert>
#include <time.h>
#include <strstream>

#include "Sdp2Session.hxx"


using Vocal::SDP::SdpTime;
using Vocal::SDP::SdpTimeRepeat;

const unsigned SdpTime::NTP_UX_DIFF = 2208988800U;


SdpTime::SdpTime()
	: repeatList()
{
    char timeStr[256];
    sprintf(timeStr, "%u", static_cast < unsigned int > (time(NULL) + NTP_UX_DIFF) );
    startTime = timeStr;
    ;
    stopTime = "0";
}    // SdpTime::SdpTime


SdpTime::SdpTime(time_t start, time_t end)
	: repeatList()
{
    char timeStr[256];
    sprintf(timeStr, "%u", static_cast < unsigned int > (start + NTP_UX_DIFF) );
    startTime = timeStr;

    sprintf(timeStr, "%u", static_cast < unsigned int > (end + NTP_UX_DIFF) );
    stopTime = timeStr;
}


///
SdpTime::SdpTime (list<Data>& timeDescriptionLines)
{
    list<Data>::iterator linecount = timeDescriptionLines.begin();
    while (linecount != timeDescriptionLines.end())
    {
        if ((*linecount)[0] == 't')
        {
            Data s = (*linecount);
	    s.parse("=");

            list<Data> tList;
	    bool finished = false;

	    while (!finished)
	    {
		Data x = s.parse(" ", &finished);
		if(finished)
		{
		    x = s;
		}
		tList.push_back(x);
	    }


            if (tList.size() != 2)
            {
                throw SdpExceptions (PARAM_NUMERR);
            }
            else
            {
                startTime = *tList.begin();
                stopTime = *(++tList.begin());
            }
        }
        else if ((*linecount)[0] == 'r')
        {
            Data s = (*linecount);
	    s.parse("=");

            list<Data> rList;
	    bool finished = false;

	    while (!finished)
	    {
		Data x = s.parse(" ", &finished);
		if(finished)
		{
		    x = s;
		}
		rList.push_back(x);
	    }

            if (rList.size() < 2)
            {
                throw SdpExceptions (PARAM_NUMERR);
            }
            else
            {
                SdpTimeRepeat stp;
                list<Data>::iterator iter;

                iter = rList.begin();
                stp.setInterval((*iter));
                ++iter;
                stp.setActiveDuration((*iter));
                ++iter;

                while (iter != rList.end())   // at least one offset
                {
                    // Push the offset on to the offsetList
                    stp.addOffset((*iter));
                    ++iter;
                }
                repeatList.push_back(stp);
            }
        }
        ++linecount;
    }
}    // SdpTime::SdpTime

///
void
SdpTime::encode (ostrstream& s)
{
    s << "t=" << startTime << ' ' << stopTime << "\r\n";

    vector < SdpTimeRepeat > ::iterator repeatIter;
    repeatIter = repeatList.begin();

    while (repeatIter != repeatList.end())
    {
        s << "r=" << (*repeatIter).getInterval() << ' '
        << (*repeatIter).getActiveDuration() ;

        if (0 == (*repeatIter).getOffsetListSize())
        {
            s << "\r\n";
        }
        else
        {
            vector < Data > offsetList;
            offsetList = (*repeatIter).getOffsetList();
            vector < Data > ::iterator offsetIter;
            offsetIter = offsetList.begin();

            while (offsetIter != offsetList.end())
            {
                s << ' ' << *offsetIter ;
                ++offsetIter;
            }
            s << "\r\n";
        }
        ++repeatIter;
    }
}    // SdpTime::encode


SdpTimeRepeat::SdpTimeRepeat()
{
    interval = "";
    activeDuration = "";
}
