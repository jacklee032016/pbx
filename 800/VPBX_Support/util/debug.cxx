/*
* $Id: debug.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include "debug.h"
#include <stdlib.h>
#ifndef NO_IOSTREAMS_
#include <iostream>
#endif

void breakpoint()
{
    return ;
}

int MY_DEBUG_LEVEL = atoi(getenv("DEBUG_LEVEL") ? getenv("DEBUG_LEVEL") : "0");

bool DEBUG_SDP = (getenv("MGCP_DEBUG_SDP") ? 1 : 0);
bool DEBUG_PARSE = (getenv("MGCP_DEBUG_PARSE") ? 1 : 0);
bool DEBUG_NTWK = (getenv("MGCP_DEBUG_NTWK") ? 1 : 0);
bool DEBUG_NTWK_TIME = (getenv("MGCP_DEBUG_NTWK_TIME") ? 1 : 0);
bool DEBUG_LOWLEVEL = (getenv("MGCP_DEBUG_LOWLEVEL") ? 1 : 0);
