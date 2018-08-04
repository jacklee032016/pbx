#ifndef TRANSCEIVERSYMBOLS_HXX_
#define TRANSCEIVERSYMBOLS_HXX_
/*
* $Log: TransceiverSymbols.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: TransceiverSymbols.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"

namespace Assist
{

    const int retransmitTimeInitial = 500; 
    const  int retransmitTimeMax = 4000; 
    const  int  sipmaxRetrans = 7;
    const  int  SIP_PORT = 5060;
    const  bool  retransmitOff = false;
    static const char TransNull[] = "";
    static const char TRANSEQUAL[]= "==";
    static const char TRANSNOTEQUAL[]= "!=";
} // namespace Assist

#endif
