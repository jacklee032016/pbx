#ifndef SDP2TIME_HXX_
#define SDP2TIME_HXX_
/*
 * $Id: Sdp2Time.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include <vector>
#include <strstream>

#include "Data.hxx"
#include "support.hxx"
#include "Data.hxx"


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Infrastructure in VOCAL related to logging.<br><br>
 */
namespace SDP
{


/// r=<repeat-interval> <active-duration> <list-of-offsets-from-start-time>
class SdpTimeRepeat
{
    public:
        ///
        SdpTimeRepeat();
        ///
        Data getInterval ()
        {
            return interval;
        }
        ///
        void setInterval (const Data& inter)
        {
            interval = inter;
        }

        ///
        Data getActiveDuration()
        {
            return activeDuration;
        }
        ///
        void setActiveDuration (const Data& duration)
        {
            activeDuration = duration;
        }

        ///
        vector < Data > getOffsetList()
        {
            return offsetList;
        }
        ///
        void addOffset (const Data& offset)
        {
            offsetList.push_back(offset);
        }
        ///
        int getOffsetListSize()
        {
            return offsetList.size();
        }

    private:
        Data interval;
        Data activeDuration;
        vector < Data > offsetList;
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
        SdpTime (list<Data>& timeDescriptionLines);

        ///
        Data getStartTime()
        {
            return startTime;
        }
        ///
        void setStartTime (const Data& start)
        {
            startTime = start;
        }

        ///
        Data getStopTime()
        {
            return stopTime;
        }
        ///
        void setStopTime (const Data& stop)
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
        Data startTime;
        Data stopTime;
        vector < SdpTimeRepeat > repeatList;
};


}


}
#endif
