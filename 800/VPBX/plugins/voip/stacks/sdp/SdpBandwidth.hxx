#ifndef _SDPBANDWIDTH_H
#define _SDPBANDWIDTH_H
/*
* $Log: SdpBandwidth.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:05:22  lizhijie
* no message
*
* $Id: SdpBandwidth.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include <vector>
#include <strstream>

#include "SdpExterns.hxx"

#include "cpLog.h"


///
enum SdpBandwidthModifierType
{
    BandwidthModifierTypeAS,  // Application Specific
    BandwidthModifierTypeCT  // Conference Total
};


/** Bandwidth
    b=<modifier>:<bandwidth-value>
 */
class SdpBandwidth
{
    public:
        ///
        SdpBandwidth()
        {
            modifier = BandwidthModifierTypeAS;
            value = 64;
        }
        ///
        SdpBandwidth(string& str);
        ///
        SdpBandwidthModifierType getModifier()
        {
            return modifier;
        }
        ///
        void setModifier(SdpBandwidthModifierType bwmodifier)
        {
            modifier = bwmodifier;
        }
        ///
        int getValue()
        {
            return value;
        }
        ///
        void setValue(int val)
        {
            value = val;
        }

        ///
        void encode (ostrstream& s);

    private:
        ///
        string modifierString();

        /// Bandwidth modifier. Default is AS (Application-Specific Maximum)
        SdpBandwidthModifierType modifier;

        /// Bandwidth value in kilobits per second. Default is 64 for AS
        int value;
};

#endif
