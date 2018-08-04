#ifndef SDPTIME_HXX_
#define SDPTIME_HXX_
/*
* $Log: SdpTime.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:05:22  lizhijie
* no message
*
* $Id: SdpTime.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include <vector>
#include <string>
#include "support.hxx"
#include "Data.hxx"
#include <strstream>

/// r=<repeat-interval> <active-duration> <list-of-offsets-from-start-time>
class SdpTimeRepeat
{
    public:
        ///
        SdpTimeRepeat();
        ///
        string getInterval ()
        {
            return interval;
        }
        ///
        void setInterval (const string& inter)
        {
            interval = inter;
        }

        ///
        string getActiveDuration()
        {
            return activeDuration;
        }
        ///
        void setActiveDuration (const string& duration)
        {
            activeDuration = duration;
        }

        ///
        vector < string > getOffsetList()
        {
            return offsetList;
        }
        ///
        void addOffset (const string& offset)
        {
            offsetList.push_back(offset);
        }
        ///
        int getOffsetListSize()
        {
            return offsetList.size();
        }

    private:
        string interval;
        string activeDuration;
        vector < string > offsetList;
};


/** t=<start-time> <stop-time>
 *  zero or more SdpTimeRepeat
 */
class SdpTime
{
    public:
        static const unsigned NTP_UX_DIFF;
        ///
        SdpTime();
	///
	SdpTime(time_t start, time_t end);
        ///
        SdpTime (split_t& timeDescriptionLines);

        ///
        Data getStartTime()
        {
            return startTime;
        }
        ///
        void setStartTime (const string& start)
        {
            startTime = start;
        }

        ///
        Data getStopTime()
        {
            return stopTime;
        }
        ///
        void setStopTime (const string& stop)
        {
            stopTime = stop;
        }

        ///
        vector < SdpTimeRepeat > getTimeRepeat()
        {
            return repeatList;
        }
        ///
        void addTimeRepeat (SdpTimeRepeat& repeat)
        {
            repeatList.push_back(repeat);
        }

        ///
        void encode (ostrstream& s);

    private:
        string startTime;
        string stopTime;
        vector < SdpTimeRepeat > repeatList;
};

#endif
