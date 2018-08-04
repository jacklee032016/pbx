/*
* $Id: DataMethodPerformanceTest.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/

#include <string>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <sys/time.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include "Data.hxx"
#include "Data2.hxx"
#include "DurationStatistic.hxx"
#include "PerformanceDb.hxx"
#include "TickCount.hxx"


using Vocal::Statistics::DurationStatistic;
using Vocal::Statistics::StatisticsDb;
using Vocal::Statistics::TickCount;



int
main( int argc, char* argv[] )
{
    StatisticsDb db;


    for(int i = 0; i < 1000; i++)
    {
	DurationStatistic stat(db, Data("ParseInvite"));
	Data tmp("this is a long test pattern-to-match this is the rest");
	Data t2;
	stat.start();
	tmp.match("pattern-to-match", &t2, true);
	stat.stop();
	stat.record();
	assert(tmp == " this is the rest");
	assert(t2 == "this is a long test ");
    }

    cout << db << endl;
    return 0;
}
