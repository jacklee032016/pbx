#ifndef SDP2ZONEADJUSTMENT_HXX_
#define SDP2ZONEADJUSTMENT_HXX_

/*
* $Log: Sdp2ZoneAdjustment.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:05:46  lizhijie
* no message
*
* $Id: Sdp2ZoneAdjustment.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include <strstream>

#include "Data.hxx"
#include "cpLog.h"
#include "Sdp2Exceptions.hxx"


/** Infrastructure common to VOCAL.
 */
namespace Assist 
{


/** Infrastructure in VOCAL related to logging.<br><br>
 */
namespace SDP
{


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
        SdpZoneAdjustment(const Data& time, const Data& offset_in)
        {
            adjustment_time = time;
            offset = offset_in;
        }
	///
        Data getAdjustmentTime()
        {
            return adjustment_time;
        }
        ///
        void setAdjustmentTime(const Data& time)
        {
            adjustment_time = time;
        }
        ///
        Data getOffset()
        {
            return offset;
        }
        ///
        void setOffset (const Data& offset_in)
        {
            offset = offset_in;
        }
        ///
        void encode (ostrstream& s);

    private:
        ////
        Data adjustment_time;
        Data offset;
};

}

}

#endif
