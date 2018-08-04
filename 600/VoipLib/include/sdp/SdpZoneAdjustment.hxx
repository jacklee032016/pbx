#ifndef SDPZONEADJUSTMENT_HXX_
#define SDPZONEADJUSTMENT_HXX_
/*
 * $Id: SdpZoneAdjustment.hxx,v 1.2 2007/03/01 20:04:27 lizhijie Exp $
 */


#include <string>
#include <strstream>
#include "cpLog.h"
#include "SdpExceptions.hxx"

/** z=<adjustment time> <offset> <adjustment time> <offset> ....
 */
class SdpZoneAdjustment
{
    public:
        ///
        SdpZoneAdjustment()
        {
            adjustment_time = offset = "";
        }
        ///
        SdpZoneAdjustment(const string& time, const string& offset_in)
        {
            adjustment_time = time;
            offset = offset_in;
        }
	///
        string getAdjustmentTime()
        {
            return adjustment_time;
        }
        ///
        void setAdjustmentTime(const string& time)
        {
            adjustment_time = time;
        }
        ///
        string getOffset()
        {
            return offset;
        }
        ///
        void setOffset (const string& offset_in)
        {
            offset = offset_in;
        }
        ///
        void encode (ostrstream& s);

    private:
        ////
        string adjustment_time;
        string offset;
};
#endif
