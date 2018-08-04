/*
* $Log: SdpTime.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SdpTime.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"
#include <cassert>
#include <time.h>
#include <strstream>
#include "SdpSession.hxx"


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
SdpTime::SdpTime (split_t& timeDescriptionLines)
{
    split_t::iterator linecount = timeDescriptionLines.begin();
    while (linecount != timeDescriptionLines.end())
    {
        if ((*linecount)[0] == 't')
        {
            string s = (*linecount);
            s.erase (0, 2);
            chomp (&s);
            split_t tList (split(s, " "));
            if (tList.size() != 2)
            {
                throw SdpExceptions (PARAM_NUMERR);
            }
            else
            {
                startTime = tList[0];
                stopTime = tList[1];
            }
        }
        else if ((*linecount)[0] == 'r')
        {
            string s = (*linecount);
            s.erase (0, 2);
            chomp (&s);
            split_t rList (split(s, " "));

            if (rList.size() < 2)
            {
                throw SdpExceptions (PARAM_NUMERR);
            }
            else
            {
                SdpTimeRepeat stp;
                split_t::iterator iter;

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
            vector < string > offsetList;
            offsetList = (*repeatIter).getOffsetList();
            vector < string > ::iterator offsetIter;
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
