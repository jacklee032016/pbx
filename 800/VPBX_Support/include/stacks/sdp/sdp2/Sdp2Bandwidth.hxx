#ifndef SDP2BANDWIDTH_HXX_
#define SDP2BANDWIDTH_HXX_
/*
* $Log: Sdp2Bandwidth.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: Sdp2Bandwidth.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include <vector>
#include <strstream>

#include "Sdp2Externs.hxx"
#include "cpLog.h"


/** Infrastructure common to VOCAL.
 */
namespace Assist 
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
