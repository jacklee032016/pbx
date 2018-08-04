/*
* $Log: Sdp2Bandwidth.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: Sdp2Bandwidth.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"
#include "Data.hxx"
#include "support.hxx"
#include <cassert>
#include <list>

#include "Sdp2Exceptions.hxx"
#include "Sdp2Bandwidth.hxx"


using Assist::SDP::SdpBandwidth;


SdpBandwidth::SdpBandwidth (Data& s)
{
    //This is of the form:
    //<modifier>:<bandwidth-value>
    list<Data> bandwidthList;

	    bool finished = false;

	    while (!finished)
	    {
		Data x = s.parse(":", &finished);
		if(finished)
		{
		    x = s;
		}
		bandwidthList.push_back(x);
	    }


    if (bandwidthList.size() < 2)
    {
        cpLog(LOG_ERR, "SdpBandwidth: Not enough parameters");
        throw SdpExceptions(PARAM_NUMERR);
    }
    else
    {
	Data x = *bandwidthList.begin();
	Data y = *(++bandwidthList.begin());

        if (x == SdpBandwidthModifierAS)
        {
            modifier = BandwidthModifierTypeAS;
        }
        else if (x == SdpBandwidthModifierCT)
        {
            modifier = BandwidthModifierTypeCT;
        }
        else if (x == SdpBandwidthModifierRR)
        {
            modifier = BandwidthModifierTypeRR;
        }
        else
        {
            cpLog(LOG_ERR, "SdpBandwidth: Undefined Modifier");
            throw SdpExceptions(UNDEF_MODIFIER);
        }

	LocalScopeAllocator lval;
        value = atoi( y.getData(lval));
    }
}


///
Data
SdpBandwidth::modifierString ()
{
    Data s;

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
        case BandwidthModifierTypeRR:
        {
            s = SdpBandwidthModifierRR;
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
