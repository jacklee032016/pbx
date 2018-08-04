#ifndef TRANSCEIVERSYMBOLS_HXX_
#define TRANSCEIVERSYMBOLS_HXX_
/*
 * $Id: TransceiverSymbols.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include "global.h"

namespace Vocal
{

    const int retransmitTimeInitial = 500; 
    const  int retransmitTimeMax = 4000; 
    const  int  sipmaxRetrans = 7;
    const  int  SIP_PORT = 5060;
    const  bool  retransmitOff = false;
    static const char TransNull[] = "";
    static const char TRANSEQUAL[]= "==";
    static const char TRANSNOTEQUAL[]= "!=";
} // namespace Vocal
#endif
