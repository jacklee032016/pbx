/*
 * $Id: SdpBandwidth.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */

#include "global.h"
#include <string>
#include "support.hxx"
#include <cassert>

#include "SdpExceptions.hxx"
#include "SdpBandwidth.hxx"


SdpBandwidth::SdpBandwidth (string& s)
{
    //This is of the form:
    //<modifier>:<bandwidth-value>
    split_t bandwidthList(split(s, ":"));

    if (bandwidthList.size() < 2)
    {
        cpLog(LOG_ERR, "SdpBandwidth: Not enough parameters");
        throw SdpExceptions(PARAM_NUMERR);
    }
    else
    {
        if (bandwidthList[0] == SdpBandwidthModifierAS)
        {
            modifier = BandwidthModifierTypeAS;
        }
        else if (bandwidthList[0] == SdpBandwidthModifierCT)
        {
            modifier = BandwidthModifierTypeCT;
        }
        else
        {
            cpLog(LOG_ERR, "SdpBandwidth: Undefined Modifier");
            throw SdpExceptions(UNDEF_MODIFIER);
        }

        value = atoi( bandwidthList[1].c_str());
    }
}


///
string
SdpBandwidth::modifierString ()
{
    string s;

    switch (modifier)
    {
        case BandwidthModifierTypeAS:
        {
            s = SdpBandwidthModifierAS;
            break;
        }
        case BandwidthModifierTypeCT:
        {
            s = SdpBandwidthModifierCT;
            break;
        }
        default:
        {
            // modifier value is corrupted
	    assert(0);
        }
    }
    return s;
}    // SdpBandwidth::modifierString


///
void
SdpBandwidth::encode (ostrstream& s)
{
    s << "b=" << modifierString() << ':' << value << "\r\n";
}    // SdpBandwidth::encode
