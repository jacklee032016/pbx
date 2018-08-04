#ifndef SDPZONEADJUSTMENT_HXX_
#define SDPZONEADJUSTMENT_HXX_
/*
* $Log: SdpZoneAdjustment.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:05:22  lizhijie
* no message
*
* $Id: SdpZoneAdjustment.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
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
