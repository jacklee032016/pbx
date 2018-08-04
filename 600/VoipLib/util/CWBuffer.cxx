/*
 * $Id: CWBuffer.cxx,v 1.2 2007/03/01 20:04:32 lizhijie Exp $
 */


#include "global.h"
#include "CWBuffer.hxx"
#include <iostream>

#if (BUFFER_STATISTICS)
BufferStatistic* BufferStatistic::instance = 0;
#endif

char* emptyStr = "";


void dumpBufferStatistics()
{
    printBufferStatistics(cerr);
}
