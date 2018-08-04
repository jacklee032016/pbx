#ifndef SDP2BANDWIDTH_HXX_
#define SDP2BANDWIDTH_HXX_

/*
 * $Id: Sdp2Bandwidth.hxx,v 1.2 2007/03/01 20:04:27 lizhijie Exp $
 */

#include <vector>
#include <strstream>

#include "Sdp2Externs.hxx"
#include "cpLog.h"


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Infrastructure in VOCAL related to logging.<br><br>
 */
namespace SDP
{


///
enum SdpBandwidthModifierType
{
    BandwidthModifierTypeAS,  // Application Specific
    BandwidthModifierTypeCT,  // Conference Total
    BandwidthModifierTypeRR  
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
        SdpBandwidth(Data& str);
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

        ///
        Data modifierString();

    private:

        /// Bandwidth modifier. Default is AS (Application-Specific Maximum)
        SdpBandwidthModifierType modifier;

        /// Bandwidth value in kilobits per second. Default is 64 for AS
        int value;
};


}


}

#endif
