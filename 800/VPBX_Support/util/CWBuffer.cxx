/*
* $Id: CWBuffer.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
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
